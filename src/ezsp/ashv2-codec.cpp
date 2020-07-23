/**
 * @file ashv2-codec.cpp
 *
 * @brief Protocol decoder/encoder for ASH version 2
 **/

#include <iostream>
#include <list>
#include <map>
#include <iomanip>

#include "ashv2-codec.h"
#include "ezsp/ezsp-protocol/ezsp-enum.h"
#include "ezsp/byte-manip.h"

#include "spi/ILogger.h"

// For debug logging only
//#include <sstream>

DEFINE_ENUM(EAshInfo, ASH_INFO, NSEZSP::AshCodec);

using NSEZSP::AshCodec;

constexpr uint8_t ASH_CANCEL_BYTE     = 0x1A;
constexpr uint8_t ASH_FLAG_BYTE       = 0x7E;
constexpr uint8_t ASH_SUBSTITUTE_BYTE = 0x18;
constexpr uint8_t ASH_XON_BYTE        = 0x11;
constexpr uint8_t ASH_OFF_BYTE        = 0x13;
constexpr uint8_t ASH_TIMEOUT         = -1;

constexpr uint32_t ASH_MAX_LENGTH     = 131;

AshCodec::AshCodec(CAshCallback* ipCb, std::function<void (void)> ackTimeoutCancelFunc) :
	pCb(ipCb),
	ackTimerCancelFunc(ackTimeoutCancelFunc),
	nextExpectedFEAckNum(0),
	frmNum(0),
	lastReceivedByNEAckNum(0),
	stateConnected(false),
	in_msg() {
}

bool AshCodec::isInConnectedState() const {
	return this->stateConnected;
}

NSSPI::ByteBuffer AshCodec::forgeResetNCPFrame(void) {
	this->nextExpectedFEAckNum = 0;
	this->lastReceivedByNEAckNum = 0;
	this->frmNum = 0;
	this->stateConnected = false;
	NSSPI::ByteBuffer lo_msg;

	if (ackTimerCancelFunc) {
		this->ackTimerCancelFunc();	/* Cancel any existing armed ack timeout */
	}
	if (this->pCb) {
		this->pCb->ashCbInfo(ASH_STATE_DISCONNECTED);
	}

	lo_msg.push_back(0xC0);

	uint16_t crc = computeCRC(lo_msg);
	lo_msg.push_back(u16_get_hi_u8(crc));
	lo_msg.push_back(u16_get_lo_u8(crc));

	lo_msg = addByteStuffing(lo_msg);

	lo_msg.insert( lo_msg.begin(), ASH_CANCEL_BYTE );

	return lo_msg;
}

NSSPI::ByteBuffer AshCodec::forgeAckFrame(void) {
	NSSPI::ByteBuffer lo_msg;

	uint8_t ashControlByte = this->lastReceivedByNEAckNum | 0x80;
	lo_msg.push_back(ashControlByte);
	//clogD << "AshCodec creating ACK(ackNum=" << static_cast<unsigned int>(u8_get_lo_nibble(ashControlByte) & 0x07U) << ")\n";

	uint16_t crc = computeCRC(lo_msg);
	lo_msg.push_back(u16_get_hi_u8(crc));
	lo_msg.push_back(u16_get_lo_u8(crc));

	return addByteStuffing(lo_msg);
}

NSSPI::ByteBuffer AshCodec::forgeDataFrame(NSSPI::ByteBuffer i_data) {
	NSSPI::ByteBuffer lo_msg;

	uint8_t ashControlByte = static_cast<uint8_t>(this->frmNum << 4) | (this->lastReceivedByNEAckNum & 0x07U);
	lo_msg.push_back(ashControlByte);
	//clogD << "AshCodec creating DATA(frmNum=" << std::dec << static_cast<unsigned int>(u8_get_hi_nibble(ashControlByte) & 0x07U)
	//      << ", ackNum=" << static_cast<unsigned int>(u8_get_lo_nibble(ashControlByte) & 0x07U) << ")\n";
	//clogD << "EZSP payload: " << i_data << "\n";
	this->frmNum++;
	this->frmNum &= 0x07U;
	this->nextExpectedFEAckNum = this->frmNum;	/* ACK value always contain the next expected frame number */

	lo_msg.append(dataRandomize(i_data));

	uint16_t crc = computeCRC(lo_msg);
	lo_msg.push_back(u16_get_hi_u8(crc));
	lo_msg.push_back(u16_get_lo_u8(crc));

	return addByteStuffing(lo_msg);
}

NSSPI::ByteBuffer AshCodec::processInterFlagStream() {

	NSSPI::ByteBuffer lo_msg;

	lo_msg.append(removeByteStuffing(this->in_msg));

	if (lo_msg.size() < 3) { /* There should be at least a Control byte and a 16-bit CRC */
		lo_msg.clear();
		clogE << "ASH frame is too short\n";
		return lo_msg;
	}
	if (computeCRC(lo_msg) != 0) {	/* CRC of whole frame including CRC itself should be 0 */
		lo_msg.clear();
		clogE << "AshCodec::decode Wrong CRC\n";
		return lo_msg;
	}
	/* Remove 2 trailing bytes (CRC16) */
	lo_msg.pop_back();
	lo_msg.pop_back();

	uint8_t ashControlByte = lo_msg.at(0);
	if ((ashControlByte & 0x80) == 0) {
		uint8_t expectedAckNum = this->nextExpectedFEAckNum;
		uint8_t remoteAckNum = u8_get_lo_nibble(ashControlByte) & 0x07U;
		uint8_t remoteFrmNum = u8_get_hi_nibble(ashControlByte) & 0x07U;
		//clogD << "AshCodec decoding DATA(frmNum=" << static_cast<unsigned int>(remoteFrmNum)
		//      << ", ackNum=" << static_cast<unsigned int>(remoteAckNum) << ")\n";

		if (expectedAckNum != remoteAckNum) {
			clogE << "Received a wrong ack num: " << +(remoteAckNum) << ", expected: " << +(expectedAckNum) << "\n";
		}
		else {
			if (this->ackTimerCancelFunc) {
				this->ackTimerCancelFunc();  /* Stop any possibly existing timer that was waiting for an ACK */
			}
		}
		/* In any case (ACK correct or not), update increase the value of the next ACK we will send */
		this->lastReceivedByNEAckNum = remoteFrmNum+1;
		this->lastReceivedByNEAckNum &= 0x07U;

		lo_msg = dataRandomize(lo_msg, 1);	/* 1 here will skip the 1st byte (ashControlByte) from result */

		/*
		// For debugging
		clogD << "Received ASH message, decoded as EZSP message (seq=" << +(static_cast<unsigned char>(lo_msg[0]))
		      << ", FC=" << +(static_cast<unsigned char>(lo_msg[1]))
		      << ", FrameID=" << +(static_cast<unsigned char>(lo_msg[2])) << "): "
		      << lo_msg << "\n";
		*/
		return lo_msg;
	}
	else if ((ashControlByte & 0x60) == 0x00) {
		// ACK
		//-- clogD << "AshCodec::decode ACK\n";
		lo_msg.clear();
		if (this->ackTimerCancelFunc) {
			this->ackTimerCancelFunc();  /* Stop any possibly existing timer that was waiting for an ACK */
		}

		if (this->pCb != nullptr) {
			pCb->ashCbInfo(ASH_ACK);
		}
	}
	else if ((ashControlByte & 0x60) == 0x20) {
		// NAK
		frmNum = ashControlByte & 0x07;

		clogD << "AshCodec::decode NACK\n";

		//LOGGER(logTRACE) << "<-- RX ASH NACK Frame !! : 0x" << QString::number(lo_msg.at(0),16).toUpper().rightJustified(2,'0');
		lo_msg.clear();
		if (this->ackTimerCancelFunc) {
			this->ackTimerCancelFunc();  /* Stop any possibly existing timer that was waiting for an ACK */
		}

		if( nullptr != pCb ) {
			pCb->ashCbInfo(ASH_NACK);
		}
	}
	else if (ashControlByte == 0xC0) {  /* RST */
		lo_msg.clear();
		if (this->ackTimerCancelFunc) {
			this->ackTimerCancelFunc();  /* Stop any possibly existing timer that was waiting for an ACK */
		}
		clogD << "AshCodec::decode RST\n";
	}
	else if (ashControlByte == 0xC1) { /* RSTACK */
		uint8_t version = lo_msg.at(1);
		uint8_t resetCode = lo_msg.at(2);
		clogD << "AshCodec::decode RSTACK v" << std::dec << static_cast<const unsigned int>(version) << ", resetCode=0x" << std::hex << std::setw(2) << std::setfill('0') << +(static_cast<unsigned char>(resetCode)) << "\n";

		if (version!=2U) {
			clogE << "Unsupported ASH version: " << std::dec << static_cast<const unsigned int>(version) << "\n";
			lo_msg.clear();
			return lo_msg;
		}

		lo_msg.clear();
		if (!this->stateConnected ) {
			if (resetCode == 0x0b /* Software reset */
			        || resetCode == 0x09 /* Run app from bootloader */
			        || resetCode == 0x02 /* Power on */
			   ) {
				this->stateConnected = true;
				if (this->ackTimerCancelFunc) {
					this->ackTimerCancelFunc();  /* Stop any possibly existing timer that was waiting for an ACK */
				}
				if (pCb) {
					pCb->ashCbInfo(ASH_STATE_CONNECTED);
				}
			}
			else {
				clogE << "Unexpected reset code: 0x" << std::hex << std::setw(2) << std::setfill('0') << +(static_cast<unsigned char>(resetCode)) << "\n";
				lo_msg.clear();
				return lo_msg;
			}
		}
	}
	else if (ashControlByte == 0xC2) {
		clogE << "AshCodec::decode ERROR\n";
		lo_msg.clear();
	}
	else {
		clogE << "AshCodec::decode UNKNOWN\n";
		lo_msg.clear();
	}
	return lo_msg;
}

std::vector<NSSPI::ByteBuffer> AshCodec::appendIncoming(NSSPI::ByteBuffer& i_data) {
	/**
	 * Specifications for the ASH frame format can be found in Silabs's document ug101-uart-gateway-protocol-reference.pdf
	 */
	bool inputError = false;
	std::vector<NSSPI::ByteBuffer> extractedPayloads;	/*!< A vector of extracted ASH payloads... there could be 0, 1 or more payloads extracted out of the current accumulated bytes+i_data */
	uint8_t val;

	while(!i_data.empty()) {
		val = i_data.front();
		i_data.erase(i_data.begin());
		switch( val ) {
		case ASH_CANCEL_BYTE:
			// Cancel Byte: Terminates a frame in progress. A Cancel Byte causes all data received since the
			// previous Flag Byte to be ignored. Note that as a special case, RST and RSTACK frames are preceded
			// by Cancel Bytes to ignore any link startup noise.
			this->in_msg.clear();
			inputError = false;
			break;
		case ASH_FLAG_BYTE:
			//-- clogD << "AshCodec::decode ASH_FLAG_BYTE\n";
			// Flag Byte: Marks the end of a frame.When a Flag Byte is received, the data received since the
			// last Flag Byte or Cancel Byte is tested to see whether it is a valid frame.
			//LOGGER(logTRACE) << "<-- RX ASH frame: VIEW ASH_FLAG_BYTE";
			if (!inputError && !this->in_msg.empty()) {
				extractedPayloads.push_back(this->processInterFlagStream());
				/* Note: if we got several ASH frames in a single incoming byte buffer, we will loop and push_back all decoded payloads inside extractedPayload */
			}
			this->in_msg.clear();
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
			if (this->in_msg.size() >= ASH_MAX_LENGTH) {
				this->in_msg.clear();
				inputError = true;
			}
			this->in_msg.push_back(val);
			break;
		}

	}

	return extractedPayloads;
}


uint16_t AshCodec::computeCRC(const NSSPI::ByteBuffer& buf) {
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

NSSPI::ByteBuffer AshCodec::removeByteStuffing(const NSSPI::ByteBuffer& i_data) {
	NSSPI::ByteBuffer result;

	// Remove byte stuffing
	bool escape = false;
	for (auto it = i_data.begin(); it != i_data.end(); ++it) {
		if (escape) {
			escape = false;
			if ((*it & 0x20) == 0) {
				result.push_back(static_cast<uint8_t>(*it | 0x20U));
			}
			else {
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

NSSPI::ByteBuffer AshCodec::addByteStuffing(const NSSPI::ByteBuffer& i_data) {

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

NSSPI::ByteBuffer AshCodec::dataRandomize(const NSSPI::ByteBuffer& i_data, uint8_t start) {
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
