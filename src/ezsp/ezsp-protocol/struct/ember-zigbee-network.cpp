/**
 * @file ember-zigbee-network.cpp
 *
 * @brief A structure containing the parameters of a ZigBee network
 */

#include <sstream>
#include <iomanip>

#include "ezsp/ezsp-protocol/struct/ember-zigbee-network.h"
#include "ezsp/byte-manip.h"

using NSEZSP::CEmberZigbeeNetwork;

CEmberZigbeeNetwork::CEmberZigbeeNetwork(const NSSPI::ByteBuffer& raw_message) :
	channel(static_cast<uint8_t>(raw_message.at(0))),
	panId(dble_u8_to_u16(raw_message.at(2), raw_message.at(1))),
	extendedPanId(0),
	allowingJoin(raw_message.at(11)!=0),
	stackProfile(static_cast<uint8_t>(raw_message.at(12))),
	nwkUpdateId(static_cast<uint8_t>(raw_message.at(13))) {
	for (unsigned int loop=0; loop<8; loop++) {
		extendedPanId = extendedPanId << 8 | raw_message.at(3U+(7-loop) /*FIXME: why reverse?*/);
	}
}

CEmberZigbeeNetwork::CEmberZigbeeNetwork(const CEmberZigbeeNetwork& other) :
	channel(other.channel),
	panId(other.panId),
	extendedPanId(other.extendedPanId),
	allowingJoin(other.allowingJoin),
	stackProfile(other.stackProfile),
	nwkUpdateId(other.nwkUpdateId) {
}

std::string CEmberZigbeeNetwork::toString() const {
	std::stringstream buf;

	buf << "EmberZigbeeNetwork: { ";
	buf << "[channel: "<< std::dec << std::setw(0) << static_cast<unsigned int>(this->channel) << "]";
	buf << "[panId: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(this->panId) << "]";
	buf << "[extendedPanId: 0x" << std::hex
	    << std::setw(8) << std::setfill('0') << u64_get_hi_u32(this->extendedPanId)
	    << std::setw(8) << std::setfill('0') << u64_get_lo_u32(this->extendedPanId) << "]";
	buf << "[allowingJoin: " << std::string(this->allowingJoin?"true":"false") << "]";
	buf << "[stackProfile: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(this->stackProfile) << "]";
	buf << "[nwkUpdateId: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(this->nwkUpdateId) << "]";
	buf << " }";

	return buf.str();
}
