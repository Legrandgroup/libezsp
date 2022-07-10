/**
 * @file zcl-frame.cpp
 *
 * @brief Handles decoding of a zcl frame
 */

#include <sstream>
#include <iomanip>

#include "ezsp/zbmessage/zcl-frame.h"

using NSEZSP::CZclFrame;

CZclFrame::CZclFrame():
	endpoint(0),
	cluster(0),
	type(0),
	command(0),
	payload() {
}

CZclFrame::CZclFrame(uint8_t endpoint, uint16_t cluster, uint8_t type, uint8_t command, NSSPI::ByteBuffer payload):
	endpoint(0),
	cluster(0),
	type(0),
	command(0),
	payload() {

	this->endpoint = endpoint;
	this->cluster = cluster;
	this->type = type;
	this->command = command;
	this->payload = payload;
}

std::string CZclFrame::String() const {
	std::stringstream buf;

	buf << "CZclFrame: { ";
	buf << "[endpoint: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(this->endpoint) << "]";
	buf << "[cluster: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(this->cluster) << "]";
	buf << "[type: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(this->type) << "]";
	buf << "[command: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(this->command) << "]";
	buf << "[payload : " << this->payload << "]";
	buf << " }";

	return buf.str();
}
