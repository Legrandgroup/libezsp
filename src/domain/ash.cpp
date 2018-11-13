/**
 * 
 * */

// #include <iostream>
#include <list>

#include "ash.h"

using namespace std;

/**
 * The receive timeout settings - min/initial/max - defined in milliseconds
 */
#define T_RX_ACK_MIN  400
#define T_RX_ACK_INIT 1600
#define T_RX_ACK_MAX 3200

#define ASH_CANCEL_BYTE     0x1A
#define ASH_FLAG_BYTE       0x7E
#define ASH_SUBSTITUTE_BYTE 0x18
#define ASH_XON_BYTE        0x11
#define ASH_OFF_BYTE        0x13
#define ASH_TIMEOUT         -1

#define ASH_MAX_LENGTH 131

CAsh::CAsh(CAshCallback *ipCb, ITimer *ipTimer)
{
    in_msg.clear();
    ackNum = 0;
    frmNum = 0;
    seq_num = 0;
    stateConnected = false;
    pCb = ipCb;
    pTimer = ipTimer;
}

void CAsh::Timeout(void)
{
    if( !stateConnected )
    {
        if( nullptr != pCb )
        {
            pCb->ashCbInfo(ASH_RESET_FAILED);
        }
    }
}

vector<uint8_t> CAsh::resetNCPFrame(void)
{
    ackNum = 0;
    frmNum = 0;
    seq_num = 0;
    stateConnected = false;
    vector<uint8_t> lo_msg;

    if( nullptr != pTimer ){ pTimer->stop(); }
    if( nullptr != pCb ){ pCb->ashCbInfo(ASH_STATE_CHANGE); }

    lo_msg.push_back(0xC0);

    uint16_t crc = computeCRC(lo_msg);
    lo_msg.push_back(static_cast<uint8_t>(crc>>8));
    lo_msg.push_back(static_cast<uint8_t>(crc&0xFF));

    lo_msg = stuffedOutputData(lo_msg);

    lo_msg.insert( lo_msg.begin(), ASH_CANCEL_BYTE );

    // start timer
    if( nullptr != pTimer )
    { 
        pTimer->start( T_RX_ACK_INIT, [&](ITimer *ipTimer){this->Timeout();} );
    }

    return lo_msg;
}

std::vector<uint8_t> CAsh::AckFrame(void)
{
  std::vector<uint8_t> lo_msg;

  lo_msg.push_back(static_cast<uint8_t>(0x80+ackNum));

  uint16_t crc = computeCRC(lo_msg);
  lo_msg.push_back(static_cast<uint8_t>(crc>>8));
  lo_msg.push_back(static_cast<uint8_t>(crc&0xFF));

  lo_msg = stuffedOutputData(lo_msg);

  // start timer
  if( nullptr != pTimer ){ pTimer->start( T_RX_ACK_INIT, [&](ITimer *ipTimer){this->Timeout();} ); }

  return lo_msg;
}

std::vector<uint8_t> CAsh::DataFrame(std::vector<uint8_t> i_data)
{
  std::vector<uint8_t> lo_msg;

  lo_msg.push_back(static_cast<uint8_t>((frmNum << 4) + ackNum) );
  frmNum = (frmNum + 1) & 0x07;

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
  if( nullptr != pTimer ){ pTimer->start( T_RX_ACK_INIT, [&](ITimer *ipTimer){this->Timeout();} ); }

  return lo_msg;
}

std::vector<uint8_t> CAsh::decode(std::vector<uint8_t> *i_data)
{
  bool inputError = false;
  std::list<uint8_t> li_data;
  std::vector<uint8_t> lo_msg;
  uint8_t val;

  // make a copy of i_data in a list
  for( size_t loop=0; loop< i_data->size(); loop++ )
  {
      li_data.push_back(i_data->at(loop));
  }

  while( !li_data.empty() && lo_msg.empty() )
  {
    val = li_data.front();
    li_data.pop_front();
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
            //-- std::cout << "CAsh::decode ASH_FLAG_BYTE" << std::endl;
          // Flag Byte: Marks the end of a frame.When a Flag Byte is received, the data received since the
          // last Flag Byte or Cancel Byte is tested to see whether it is a valid frame.
          //LOGGER(logTRACE) << "<-- RX ASH frame: VIEW ASH_FLAG_BYTE";
          if (!inputError && !in_msg.empty()) {
            if( in_msg.size() >= 3 )
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
                  lo_msg.push_back(data);
              }

              // Check CRC
              if (computeCRC(lo_msg) != 0) {
                  lo_msg.clear();
                  //-- std::cout << "CAsh::decode Wrong CRC" << std::endl;
              }
              else
              {
                if ((lo_msg.at(0) & 0x80) == 0) {
                  // DATA;
                  //-- std::cout << "CAsh::decode DATA" << std::endl;

                  // update ack number, use incoming frm number
                  ackNum = ((lo_msg.at(0)>>4&0x07) + 1) & 0x07;


                  lo_msg = dataRandomise(lo_msg,1);

                  if( 0xFF == lo_msg.at(2) )
                  {
                    // WARNING for all frames except "VersionRequest" frame, add exteded header
                    lo_msg.erase(lo_msg.begin()+2);
                    lo_msg.erase(lo_msg.begin()+2);
                  }

                }
                else if ((lo_msg.at(0) & 0x60) == 0x00) {
                  // ACK;
                  //-- std::cout << "CAsh::decode ACK" << std::endl;
                  //LOGGER(logTRACE) << "<-- RX ASH ACK Frame !! ";
                  lo_msg.clear();
                  if( nullptr != pTimer ){ pTimer->stop(); }

                  if( nullptr != pCb ) { pCb->ashCbInfo(ASH_ACK); }
                }
                else if ((lo_msg.at(0) & 0x60) == 0x20) {
                  // NAK;
                  frmNum = lo_msg.at(0) & 0x07;

                  //-- std::cout << "CAsh::decode NACK" << std::endl;

                  //LOGGER(logTRACE) << "<-- RX ASH NACK Frame !! : 0x" << QString::number(lo_msg.at(0),16).toUpper().rightJustified(2,'0');
                  lo_msg.clear();
                  if( nullptr != pTimer ){ pTimer->stop(); }
                  
                  if( nullptr != pCb ) { pCb->ashCbInfo(ASH_NACK); }
                }
                else if (lo_msg.at(0) == 0xC0) {
                  // RST;
                  lo_msg.clear();
                  //LOGGER(logTRACE) << "<-- RX ASH RST Frame !! ";
                  //-- std::cout << "CAsh::decode RST" << std::endl;
                }
                else if (lo_msg.at(0) == 0xC1) {
                  // RSTACK;
                  //LOGGER(logTRACE) << "<-- RX ASH RSTACK Frame !! ";
                  //-- std::cout << "CAsh::decode RSTACK" << std::endl;

                  lo_msg.clear();
                  if( !stateConnected )
                  {
                    /** \todo : add some test to verify it is a software reset and ash protocol version is 2 */
                    if( nullptr != pTimer ){ pTimer->stop(); }
                    stateConnected = true;
                    if( nullptr != pCb ){ pCb->ashCbInfo(ASH_STATE_CHANGE); }
                  }
                }
                else if (lo_msg.at(0) == 0xC2) {
                  // ERROR;
                  //LOGGER(logTRACE) << "<-- RX ASH ERROR Frame !! ";
                  //-- std::cout << "CAsh::decode ERROR" << std::endl;
                  lo_msg.clear();
                }
                else
                {
                  //LOGGER(logTRACE) << "<-- RX ASH Unknown !! ";
                  //-- std::cout << "CAsh::decode UNKNOWN" << std::endl;
                  lo_msg.clear();
                }
              }
            }
            else
            {
              //LOGGER(logTRACE) << "<-- RX ASH too short !! ";
            }
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

uint16_t CAsh::computeCRC( vector<uint8_t> i_msg )
{
  uint16_t lo_crc = 0xFFFF; // initial value
  uint16_t polynomial = 0x1021; // 0001 0000 0010 0001 (0, 5, 12)

  for (std::size_t cnt = 0; cnt < i_msg.size(); cnt++) {
      for (auto i = 0; i < 8; i++) {
          bool bit = ((i_msg.at(cnt) >> (7 - i) & 1) == 1);
          bool c15 = ((lo_crc >> 15 & 1) == 1);
          lo_crc = static_cast<uint16_t>(lo_crc << 1U);
          if (c15 ^ bit) {
              lo_crc ^= polynomial;
          }
      }
  }

  lo_crc &= 0xffff;

  return lo_crc;
}

vector<uint8_t> CAsh::stuffedOutputData(vector<uint8_t> i_msg)
{
  vector<uint8_t> lo_msg;

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

vector<uint8_t> CAsh::dataRandomise(vector<uint8_t> i_data, uint8_t start)
{
    vector<uint8_t> lo_data;

    // Randomise the data
    uint8_t rand = 0x42;
    for (uint8_t cnt = start; cnt < i_data.size(); cnt++) {
        lo_data.push_back(i_data.at(cnt) ^ rand);

        if ((rand & 0x01) == 0) {
            rand = static_cast<uint8_t>(rand >> 1);
        } else {
            rand = static_cast<uint8_t>((rand >> 1) ^ 0xb8);
        }
    }

    return lo_data;
}