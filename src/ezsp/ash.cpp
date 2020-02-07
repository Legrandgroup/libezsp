/**
 * @file ash.cpp
 *
 * @brief ASH protocol decoder/encoder
 **/

#include <iostream>
#include <list>
#include <map>
#include <iomanip>

#include "ash.h"

#include "spi/ILogger.h"

// For debug logging only
//#include <sstream>

using NSEZSP::CAsh;

/**
 * The receive timeout settings - min/initial/max - defined in milliseconds
 */
constexpr size_t T_RX_ACK_MIN = 400;
constexpr size_t T_RX_ACK_INIT = 1600;
constexpr size_t T_RX_ACK_MAX = 3200;
constexpr size_t T_ACK_ASH_RESET = 5000;

constexpr uint8_t ASH_CANCEL_BYTE     = 0x1A;
constexpr uint8_t ASH_FLAG_BYTE       = 0x7E;
constexpr uint8_t ASH_SUBSTITUTE_BYTE = 0x18;
constexpr uint8_t ASH_XON_BYTE        = 0x11;
constexpr uint8_t ASH_OFF_BYTE        = 0x13;
constexpr uint8_t ASH_TIMEOUT         = -1;

constexpr uint32_t ASH_MAX_LENGTH     = 131;

CAsh::CAsh(CAshCallback *ipCb, NSSPI::TimerBuilder &i_timer_factory) :
	ackNum(0),
	frmNum(0),
	seq_num(0),
	stateConnected(false),
	timer(i_timer_factory.create()),
	pCb(ipCb),
	in_msg()
{
}

void CAsh::trigger(NSSPI::ITimer* triggeringTimer)
{
    if( !stateConnected )
    {
        if( nullptr != pCb )
        {
            pCb->ashCbInfo(ASH_RESET_FAILED);
        }
    }
    else
    {
        clogE << "ASH timeout while connected\n";
    }
}

NSSPI::ByteBuffer CAsh::resetNCPFrame(void)
{
    ackNum = 0;
    frmNum = 0;
    seq_num = 0;
    stateConnected = false;
    NSSPI::ByteBuffer lo_msg;

    timer->stop();
    if( nullptr != pCb ){ pCb->ashCbInfo(ASH_STATE_CHANGE); }

    lo_msg.push_back(0xC0);

    uint16_t crc = computeCRC(lo_msg);
    lo_msg.push_back(static_cast<uint8_t>(crc>>8));
    lo_msg.push_back(static_cast<uint8_t>(crc&0xFF));

    lo_msg = stuffedOutputData(lo_msg);

    lo_msg.insert( lo_msg.begin(), ASH_CANCEL_BYTE );

    // start timer
    timer->start( T_ACK_ASH_RESET, this);

    return lo_msg;
}

std::string CAsh::EAshInfoToString( EAshInfo in )
{
    const std::map<EAshInfo,std::string> MyEnumStrings {
        { ASH_RESET_FAILED, "ASH_RESET_FAILED" },
        { ASH_ACK, "ASH_ACK" },
        { ASH_NACK, "ASH_NACK" },
        { ASH_STATE_CHANGE, "ASH_STATE_CHANGE" },
    };
    auto   it  = MyEnumStrings.find(in);
    return it == MyEnumStrings.end() ? "OUT_OF_RANGE" : it->second;      
}

NSSPI::ByteBuffer CAsh::AckFrame(void)
{
  NSSPI::ByteBuffer lo_msg;

  lo_msg.push_back(static_cast<uint8_t>(0x80+ackNum));

  uint16_t crc = computeCRC(lo_msg);
  lo_msg.push_back(static_cast<uint8_t>(crc>>8));
  lo_msg.push_back(static_cast<uint8_t>(crc&0xFF));

  lo_msg = stuffedOutputData(lo_msg);

  return lo_msg;
}

NSSPI::ByteBuffer CAsh::DataFrame(NSSPI::ByteBuffer i_data)
{
  NSSPI::ByteBuffer lo_msg;

  lo_msg.push_back(static_cast<uint8_t>(frmNum << 4) + ackNum);
  frmNum = (static_cast<uint8_t>(frmNum + 1)) & 0x07;

  if( 0 != i_data.at(0) )
  {
    // WARNING for all frames except "VersionRequest" frame, add exteded header
    i_data.insert(i_data.begin(),0);
    i_data.insert(i_data.begin(),0xFF);
  }

  // insert frm control
  i_data.insert(i_data.begin(),0);
  // insert seq number
  i_data.insert(i_data.begin(),seq_num++);


  i_data = dataRandomise(i_data,0);
  for (auto &val : i_data)
  {
      lo_msg.push_back(val);
  }

  uint16_t crc = computeCRC(lo_msg);
  lo_msg.push_back(static_cast<uint8_t>(crc>>8));
  lo_msg.push_back(static_cast<uint8_t>(crc&0xFF));

  lo_msg = stuffedOutputData(lo_msg);

  // start timer
  timer->stop();
  timer->start( T_RX_ACK_INIT, this);

  return lo_msg;
}

void CAsh::clean_flag(NSSPI::ByteBuffer& lo_msg)
{
  // Remove byte stuffing
  bool escape = false;
  for (auto &data : in_msg) {
    if (escape) {
      escape = false;
      if ((data & 0x20) == 0) {
        data = static_cast<uint8_t>(data + 0x20);
      } else {
        data = static_cast<uint8_t>(data & 0xDF);
      }
    } else if (data == 0x7D) {
      escape = true;
      continue;
    }
    else {
	}
    lo_msg.push_back(data);
  }
}

void CAsh::decode_flag(NSSPI::ByteBuffer& lo_msg)
{
  clean_flag(lo_msg);
  // Check CRC
  if (computeCRC(lo_msg) != 0) {
	  lo_msg.clear();
	  clogD << "CAsh::decode Wrong CRC" << std::endl;
	  return;
  }
  if ((lo_msg.at(0) & 0x80) == 0) {
    // DATA;
    //-- clogD << "CAsh::decode DATA" << std::endl;

    // update ack number, use incoming frm number
    ackNum = (static_cast<uint8_t>(lo_msg.at(0)>>4) & 0x07) + 1;
    ackNum &= 0x07;


    lo_msg = dataRandomise(lo_msg,1);

    if (lo_msg.size()<3)
    {
      clogE << "EZSP data message is too short, discarding\n";
    }
    else
    {
      if( 0xFF == lo_msg.at(2) )  /* 0xff as frame ID means we use an extended header, where frame ID will actually be shifted 2 bytes away... so we just delete those two bytes */
      {
        lo_msg.erase(lo_msg.begin()+2);
        lo_msg.erase(lo_msg.begin()+2);
      }
    }
    /*
    // For debugging
    std::stringstream msg;
    msg << "Received ASH message, decoded as EZSP message (seq=" << +(static_cast<unsigned char>(lo_msg[0])) << " , FC=" << +(static_cast<unsigned char>(lo_msg[1])) << " , FrameID=" << +(static_cast<unsigned char>(lo_msg[2])) << "):";
    for (size_t loop=1; loop<lo_msg.size(); loop++) {
      msg << " " << std::hex << std::setw(2) << std::setfill('0') <<
          +(static_cast<unsigned char>(lo_msg[loop]));
    }
    msg << "\n";
    clogD << msg.str();
    */
  }
  else if ((lo_msg.at(0) & 0x60) == 0x00) {
    // ACK;
    //-- clogD << "CAsh::decode ACK" << std::endl;
    lo_msg.clear();
    timer->stop();

    if( nullptr != pCb ) { pCb->ashCbInfo(ASH_ACK); }
  }
  else if ((lo_msg.at(0) & 0x60) == 0x20) {
    // NAK;
    frmNum = lo_msg.at(0) & 0x07;

    clogD << "CAsh::decode NACK" << std::endl;

    //LOGGER(logTRACE) << "<-- RX ASH NACK Frame !! : 0x" << QString::number(lo_msg.at(0),16).toUpper().rightJustified(2,'0');
    lo_msg.clear();
    timer->stop();

    if( nullptr != pCb ) { pCb->ashCbInfo(ASH_NACK); }
  }
  else if (lo_msg.at(0) == 0xC0) {  /* RST */
    lo_msg.clear();
    clogD << "CAsh::decode RST" << std::endl;
  }
  else if (lo_msg.at(0) == 0xC1) { /* RSTACK */
    uint8_t version = lo_msg.at(1);
    uint8_t resetCode = lo_msg.at(2);
    clogD << "CAsh::decode RSTACK v" << std::dec << static_cast<const unsigned int>(version) << ", resetCode=0x" << std::hex << std::setw(2) << std::setfill('0') << +(static_cast<unsigned char>(resetCode)) << "\n";

    if (version!=2U) {
      clogE << "Unsupported ASH version: " << std::dec << static_cast<const unsigned int>(version) << "\n";
      lo_msg.clear();
      return;
    }

    lo_msg.clear();
    if( !stateConnected ) {
      if (resetCode == 0x0b /* Software reset */
          || resetCode == 0x09 /* Run app from bootloader */
          || resetCode == 0x02 /* Power on */
         ) {
        stateConnected = true;
        timer->stop();
        if (nullptr != pCb) { pCb->ashCbInfo(ASH_STATE_CHANGE); }
      }
      else {
        clogE << "Unexpected reset code: 0x" << std::hex << std::setw(2) << std::setfill('0') << +(static_cast<unsigned char>(resetCode)) << "\n";
        lo_msg.clear();
        return;
      }
    }
  }
  else if (lo_msg.at(0) == 0xC2) {
    clogE << "CAsh::decode ERROR" << std::endl;
    lo_msg.clear();
  }
  else
  {
    clogE << "CAsh::decode UNKNOWN" << std::endl;
    lo_msg.clear();
  }
}

NSSPI::ByteBuffer CAsh::decode(NSSPI::ByteBuffer& i_data)
{
  /**
   * Specifications for the ASH frame format can be found in Silabs's document ug101-uart-gateway-protocol-reference.pdf
   */
  bool inputError = false;
  //std::list<uint8_t> li_data;
  NSSPI::ByteBuffer lo_msg;
  uint8_t val;

  while( !i_data.empty() && lo_msg.empty() )
  {
    val = i_data.front();
    i_data.erase(i_data.begin());
    switch( val )
    {
      case ASH_CANCEL_BYTE:
          // Cancel Byte: Terminates a frame in progress. A Cancel Byte causes all data received since the
          // previous Flag Byte to be ignored. Note that as a special case, RST and RSTACK frames are preceded
          // by Cancel Bytes to ignore any link startup noise.
          in_msg.clear();
          inputError = false;
          break;
      case ASH_FLAG_BYTE:
            //-- clogD << "CAsh::decode ASH_FLAG_BYTE" << std::endl;
          // Flag Byte: Marks the end of a frame.When a Flag Byte is received, the data received since the
          // last Flag Byte or Cancel Byte is tested to see whether it is a valid frame.
          //LOGGER(logTRACE) << "<-- RX ASH frame: VIEW ASH_FLAG_BYTE";
          if (!inputError && !in_msg.empty() && ( in_msg.size() >= 3 )) {
            decode_flag(lo_msg);
          }
          in_msg.clear();
          inputError = false;
          break;
      case ASH_SUBSTITUTE_BYTE:
          // Substitute Byte: Replaces a byte received with a low-level communication error (e.g., framing
          // error) from the UART.When a Substitute Byte is processed, the data between the previous and the
          // next Flag Bytes is ignored.
          inputError = true;
          break;
      case ASH_XON_BYTE:
          // XON: Resume transmissionUsed in XON/XOFF flow control. Always ignored if received by the NCP.
          break;
      case ASH_OFF_BYTE:
          // XOFF: Stop transmissionUsed in XON/XOFF flow control. Always ignored if received by the NCP.
          break;
/*          
      case ASH_TIMEOUT:
          break;
*/          
      default:
          if (in_msg.size() >= ASH_MAX_LENGTH) {
              in_msg.clear();
              inputError = true;
          }
          in_msg.push_back(val);
          break;
    }

  }

  return lo_msg;
}


/**
 * PRIVATE FUNCTION
 */

uint16_t CAsh::computeCRC( NSSPI::ByteBuffer i_msg )
{
  uint16_t lo_crc = 0xFFFF; // initial value
  uint16_t polynomial = 0x1021; // 0001 0000 0010 0001 (0, 5, 12)

  for (std::size_t cnt = 0; cnt < i_msg.size(); cnt++) {
      for (uint8_t i = 0; i < 8; i++) {
          bool bit = ((static_cast<uint8_t>(i_msg.at(cnt) >> static_cast<uint8_t>(7 - i)) & 1) == 1);
          bool c15 = ((static_cast<uint8_t>(lo_crc >> 15) & 1) == 1);
          lo_crc = static_cast<uint16_t>(lo_crc << 1U);
          if (c15 != bit) {
              lo_crc ^= polynomial;
          }
      }
  }

  lo_crc &= 0xffff;

  return lo_crc;
}

NSSPI::ByteBuffer CAsh::stuffedOutputData(NSSPI::ByteBuffer i_msg)
{
  NSSPI::ByteBuffer lo_msg;

  for (std::size_t cnt = 0; cnt < i_msg.size(); cnt++) {
      switch (i_msg.at(cnt)) {
          case 0x7E:
              lo_msg.push_back( 0x7D );
              lo_msg.push_back( 0x5E );
              break;
          case 0x7D:
              lo_msg.push_back( 0x7D );
              lo_msg.push_back( 0x5D );
              break;
          case 0x11:
              lo_msg.push_back( 0x7D );
              lo_msg.push_back( 0x31 );
              break;
          case 0x13:
              lo_msg.push_back( 0x7D );
              lo_msg.push_back( 0x33 );
              break;
          case 0x18:
              lo_msg.push_back( 0x7D );
              lo_msg.push_back( 0x38 );
              break;
          case 0x1A:
              lo_msg.push_back( 0x7D );
              lo_msg.push_back( 0x3A );
              break;
          default:
              lo_msg.push_back( i_msg.at(cnt) );
              break;
      }
  }
  lo_msg.push_back( 0x7E );


  return lo_msg;
}

NSSPI::ByteBuffer CAsh::dataRandomise(NSSPI::ByteBuffer i_data, uint8_t start)
{
    NSSPI::ByteBuffer lo_data;

    // Randomise the data
    uint8_t data = 0x42;
    for (uint8_t cnt = start; cnt < i_data.size(); cnt++) {
        lo_data.push_back(i_data.at(cnt) ^ data);

        if ((data & 0x01) == 0) {
            data = static_cast<uint8_t>(data >> 1);
        } else {
            data = (static_cast<uint8_t>(data >> 1) ^ static_cast<uint8_t>(0xb8));
        }
    }

    return lo_data;
}
