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
#include "ezsp/ezsp-protocol/ezsp-enum.h"
#include "ezsp/byte-manip.h"

#include "spi/ILogger.h"

// For debug logging only
//#include <sstream>

DEFINE_ENUM(EAshInfo, ASH_INFO, NSEZSP::CAsh);

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

CAsh::CAsh(CAshCallback* ipCb, const NSSPI::TimerBuilder& i_timer_builder) :
	ackNum(0),
	frmNum(0),
	ezspSeqNum(0),
	stateConnected(false),
	ackTimer(i_timer_builder.create()),
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

NSSPI::ByteBuffer CAsh::resetNCPFrame(void) {
	this->ackNum = 0;
	this->frmNum = 0;
	this->ezspSeqNum = 0;
	this->stateConnected = false;
    NSSPI::ByteBuffer lo_msg;

	this->ackTimer->stop();
	if (this->pCb != nullptr) {
		pCb->ashCbInfo(ASH_STATE_CHANGE);
	}

    lo_msg.push_back(0xC0);

    uint16_t crc = computeCRC(lo_msg);
	lo_msg.push_back(u16_get_hi_u8(crc));
	lo_msg.push_back(u16_get_lo_u8(crc));

	lo_msg = addByteStuffing(lo_msg);

    lo_msg.insert( lo_msg.begin(), ASH_CANCEL_BYTE );

	// start timer
	this->ackTimer->start( T_ACK_ASH_RESET, this);

    return lo_msg;
}

NSSPI::ByteBuffer CAsh::AckFrame(void)
{
  NSSPI::ByteBuffer lo_msg;

	uint8_t ashControlByte = this->ackNum | 0x80;
	lo_msg.push_back(ashControlByte);
	clogD << "CAsh sending ACK(ackNum=" << static_cast<unsigned int>(u8_get_lo_nibble(ashControlByte) & 0x07U) << ")\n";

  uint16_t crc = computeCRC(lo_msg);
	lo_msg.push_back(u16_get_hi_u8(crc));
	lo_msg.push_back(u16_get_lo_u8(crc));

	return addByteStuffing(lo_msg);
}

NSSPI::ByteBuffer CAsh::DataFrame(NSSPI::ByteBuffer i_data)
{
  NSSPI::ByteBuffer lo_msg;

  /*
  clogD << "Going to send ASH message, encoding EZSP message (seq=" << +(static_cast<unsigned char>(seq_num))
        << ", " << +(static_cast<unsigned char>(frmNum))
        << ", FC=0): " << NSSPI::Logger::byteSequenceToString(li_data) << "\n"; // Note FC is hardcoded to 0 below
  */
 
  uint8_t ashControlByte = static_cast<uint8_t>(frmNum << 4) + ackNum;
  lo_msg.push_back(ashControlByte);
	clogD << "CAsh sending DATA(frmNum=" << static_cast<unsigned int>(u8_get_hi_nibble(ashControlByte) & 0x07U)
	      << ", ackNum=" << static_cast<unsigned int>(u8_get_lo_nibble(ashControlByte) & 0x07U)
	      << ", seqNum=" << static_cast<unsigned int>(this->ezspSeqNum) << ")\n";
	this->frmNum++;
	this->frmNum &= 0x07;

	if (i_data.at(0) != NSEZSP::EEzspCmd::EZSP_VERSION) {
		/* For all frames except "VersionRequest" frame, prepend with the extended header 0xff 0x00 */
		i_data.insert(i_data.begin(),0);
		i_data.insert(i_data.begin(),0xFF);
	}

	// Prepend the frame control byte 0x00
  i_data.insert(i_data.begin(),0);
	// Insert EZSP seq number
	i_data.insert(i_data.begin(),this->ezspSeqNum++);


	lo_msg.append(dataRandomize(i_data));

  uint16_t crc = computeCRC(lo_msg);
	lo_msg.push_back(u16_get_hi_u8(crc));
	lo_msg.push_back(u16_get_lo_u8(crc));

	// start timer
	this->ackTimer->stop();
	this->ackTimer->start(T_RX_ACK_INIT, this);

	return addByteStuffing(lo_msg);
}

void CAsh::decode_flag(NSSPI::ByteBuffer& lo_msg) {
	lo_msg.append(removeByteStuffing(this->in_msg));  /* FIXME: we are using this->in_msg here, we should pass it as arg instead */
  // Check CRC
  if (computeCRC(lo_msg) != 0) {
	  lo_msg.clear();
	  clogD << "CAsh::decode Wrong CRC" << std::endl;
	  return;
  }
	uint8_t ashControlByte = lo_msg.at(0);
	if ((ashControlByte & 0x80) == 0) {
		uint8_t expectedAckNum = this->ackNum;
		uint8_t remoteAckNum = ashControlByte;
		remoteAckNum >>= 4;
		remoteAckNum &= 0x07;
		clogD << "CAsh received DATA(frmNum=" << static_cast<unsigned int>(u8_get_hi_nibble(ashControlByte) & 0x07U)
		      << ", ackNum=" << static_cast<unsigned int>(u8_get_lo_nibble(ashControlByte) & 0x07U) << ")\n";

		if (expectedAckNum != remoteAckNum) {
			clogE << "Received a wrong ack num: " << +(remoteAckNum) << ", expected: " << +(expectedAckNum) << "\n";
		}
		else {
			this->ackTimer->stop();  /* Stop any possibly existing timer that was waiting for an ACK */
		}
		/* In any case (ACK correct or not), update increase our frame number for the next transmition */
		this->ackNum = remoteAckNum;
		this->ackNum++;
		this->ackNum &= 0x07;

		lo_msg = dataRandomize(lo_msg,1);

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
    clogD << "Received ASH message, decoded as EZSP message (seq=" << +(static_cast<unsigned char>(lo_msg[0]))
          << ", FC=" << +(static_cast<unsigned char>(lo_msg[1]))
          << ", FrameID=" << +(static_cast<unsigned char>(lo_msg[2])) << "): "
          << NSSPI::Logger::byteSequenceToString(lo_msg) << "\n";
    */
  }
	else if ((ashControlByte & 0x60) == 0x00) {
		// ACK;
    //-- clogD << "CAsh::decode ACK" << std::endl;
    lo_msg.clear();
		this->ackTimer->stop();

		if (this->pCb != nullptr) {
			pCb->ashCbInfo(ASH_ACK);
		}
	}
	else if ((ashControlByte & 0x60) == 0x20) {
		// NAK;
		frmNum = ashControlByte & 0x07;

    clogD << "CAsh::decode NACK" << std::endl;

    //LOGGER(logTRACE) << "<-- RX ASH NACK Frame !! : 0x" << QString::number(lo_msg.at(0),16).toUpper().rightJustified(2,'0');
    lo_msg.clear();
		this->ackTimer->stop();

		if( nullptr != pCb ) {
			pCb->ashCbInfo(ASH_NACK);
		}
	}
	else if (ashControlByte == 0xC0) {  /* RST */
		lo_msg.clear();
		this->ackTimer->stop();
		clogD << "CAsh::decode RST" << std::endl;
  }
	else if (ashControlByte == 0xC1) { /* RSTACK */
    uint8_t version = lo_msg.at(1);
    uint8_t resetCode = lo_msg.at(2);
    clogD << "CAsh::decode RSTACK v" << std::dec << static_cast<const unsigned int>(version) << ", resetCode=0x" << std::hex << std::setw(2) << std::setfill('0') << +(static_cast<unsigned char>(resetCode)) << "\n";

    if (version!=2U) {
      clogE << "Unsupported ASH version: " << std::dec << static_cast<const unsigned int>(version) << "\n";
      lo_msg.clear();
      return;
    }

    lo_msg.clear();
		if (!this->stateConnected ) {
      if (resetCode == 0x0b /* Software reset */
          || resetCode == 0x09 /* Run app from bootloader */
          || resetCode == 0x02 /* Power on */
         ) {
				this->stateConnected = true;
				this->ackTimer->stop();
        if (nullptr != pCb) { pCb->ashCbInfo(ASH_STATE_CHANGE); }
      }
      else {
        clogE << "Unexpected reset code: 0x" << std::hex << std::setw(2) << std::setfill('0') << +(static_cast<unsigned char>(resetCode)) << "\n";
        lo_msg.clear();
        return;
      }
    }
  }
  else if (ashControlByte == 0xC2) {
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


uint16_t CAsh::computeCRC(const NSSPI::ByteBuffer& buf) {
  uint16_t lo_crc = 0xFFFF; // initial value
  uint16_t polynomial = 0x1021; // 0001 0000 0010 0001 (0, 5, 12)

	for (std::size_t cnt = 0; cnt < buf.size(); cnt++) {
      for (uint8_t i = 0; i < 8; i++) {
			bool bit = ((static_cast<uint8_t>(buf.at(cnt) >> static_cast<uint8_t>(7 - i)) & 1) == 1);
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

NSSPI::ByteBuffer CAsh::removeByteStuffing(const NSSPI::ByteBuffer& i_data) {
	NSSPI::ByteBuffer result;

	// Remove byte stuffing
	bool escape = false;
	for (auto it = i_data.begin(); it != i_data.end(); ++it) {
		if (escape) {
			escape = false;
			if ((*it & 0x20) == 0) {
				result.push_back(static_cast<uint8_t>(*it | 0x20U));
			} else {
				result.push_back(static_cast<uint8_t>(*it & 0xDFU));
			}
		}
		else { // escape==false
			if (*it == 0x7D) {
				escape = true;
			}
			else {
				result.push_back(*it);  // Non-stuffed byte is copied over as-is
			}
		}
	}
	return result;
}

NSSPI::ByteBuffer CAsh::addByteStuffing(const NSSPI::ByteBuffer& i_data) {

	NSSPI::ByteBuffer result;

	for (auto it = i_data.begin(); it != i_data.end(); ++it) {
		switch (*it) {
		case 0x7EU:
			result.push_back(0x7DU);
			result.push_back(0x5EU);
			break;
		case 0x7DU:
			result.push_back(0x7DU);
			result.push_back(0x5DU);
			break;
		case 0x11U:
			result.push_back(0x7DU);
			result.push_back(0x31U);
			break;
		case 0x13U:
			result.push_back(0x7DU);
			result.push_back(0x33U);
			break;
		case 0x18U:
			result.push_back(0x7DU);
			result.push_back(0x38U);
			break;
		case 0x1AU:
			result.push_back(0x7DU);
			result.push_back(0x3AU);
			break;
		default:
			result.push_back(*it);
			break;
		}
	}
	result.push_back(0x7EU);

	return result;
}

NSSPI::ByteBuffer CAsh::dataRandomize(const NSSPI::ByteBuffer& i_data, uint8_t start) {
	NSSPI::ByteBuffer result;

	// Randomise the data
	uint8_t lfsrByte = 0x42;
	for (uint8_t cnt = start; cnt < i_data.size(); cnt++) {
		result.push_back(i_data.at(cnt) ^ lfsrByte);

		/* Now, compute the next LFSR byte */
		bool lfsrByteBit0 = lfsrByte & 0x01;
		lfsrByte >>= 1;
		if (lfsrByteBit0) {
			lfsrByte ^= static_cast<uint8_t>(0xb8U);
		}
	}

	return result;
}
