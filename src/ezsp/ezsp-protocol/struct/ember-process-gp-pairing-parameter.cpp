/**
 * @file ember-process-gp-pairing-parameter.cpp
 *
 * @brief Parameters for gpProxyTableProcessGpPairing (0xC9) command.
 *
 * Reference: docs-14-0563-16-batt-green-power-spec_ProxyBasic.pdf
 */

#include <sstream>
#include <iomanip>

#include "ember-process-gp-pairing-parameter.h"
#include "ezsp/byte-manip.h"
#include "spi/ILogger.h"

using NSEZSP::CProcessGpPairingParam;
using NSEZSP::CEmberGpSinkTableOption;

CProcessGpPairingParam::CProcessGpPairingParam(uint32_t i_gpd_source_id) :
	options(CEmberGpSinkTableOption(0), false, true, 0, 0, false, false, false),
	addr(i_gpd_source_id),
	commMode(),
	sinkNetworkAddress(),
	sinkGroupId(),
	assignedAlias(),
	sinkIeeeAddress(),
	gpdKey(),
	gpdSecurityFrameCounter(),
	forwardingRadius() {

}

CProcessGpPairingParam::CProcessGpPairingParam(CEmberGpSinkTableEntryStruct i_sink_table_entry, bool i_add_sink, bool i_remove_gpd,
        uint16_t i_sinkNetworkAddress, EmberEUI64 i_sinkIeeeAddress ) :
	options(i_sink_table_entry.getOption(), i_add_sink, i_remove_gpd, i_sink_table_entry.getSecurityLevel(), i_sink_table_entry.getSecurityKeyType(), true, true, true),
	addr(i_sink_table_entry.getGpdAddr()),
	commMode(i_sink_table_entry.getOption().getCommunicationMode()),
	sinkNetworkAddress(i_sinkNetworkAddress),
	sinkGroupId(static_cast<uint16_t>(i_sink_table_entry.getGpdAddr().getSourceId()&0xFFFF)),
	assignedAlias(i_sink_table_entry.getAssignedAlias()),
	sinkIeeeAddress(i_sinkIeeeAddress),
	gpdKey(i_sink_table_entry.getGpdKey()),
	gpdSecurityFrameCounter(i_sink_table_entry.getSecurityFrameCounter()),
	forwardingRadius(i_sink_table_entry.getGroupcastRadius()) {

}


NSSPI::ByteBuffer CProcessGpPairingParam::get() const {
	NSSPI::ByteBuffer lo_out;

	// The options field of the GP Pairing command
	uint32_t l_option = options.get();
	lo_out.push_back(u32_get_byte0(l_option));
	lo_out.push_back(u32_get_byte1(l_option));
	lo_out.push_back(u32_get_byte2(l_option));
	lo_out.push_back(u32_get_byte3(l_option));
	// The addressing info of the target GPD.
	NSSPI::ByteBuffer l_gpd_addr = addr.getRaw();
	lo_out.insert(lo_out.end(),l_gpd_addr.begin(),l_gpd_addr.end());
	// The communication mode of the GP Sink.
	lo_out.push_back(commMode);
	// The network address of the GP Sink.
	lo_out.push_back(u16_get_lo_u8(sinkNetworkAddress));
	lo_out.push_back(u16_get_hi_u8(sinkNetworkAddress));
	// The group ID of the GP Sink.
	lo_out.push_back(u16_get_lo_u8(sinkGroupId));
	lo_out.push_back(u16_get_hi_u8(sinkGroupId));
	// The alias assigned to the GPD.
	lo_out.push_back(u16_get_lo_u8(assignedAlias));
	lo_out.push_back(u16_get_hi_u8(assignedAlias));
	// The IEEE address of the GP Sink.
	lo_out.insert(lo_out.end(),sinkIeeeAddress.begin(),sinkIeeeAddress.end());
	// The key to use for GPD.
	lo_out.insert(lo_out.end(),gpdKey.begin(),gpdKey.end());
	// The security frame counter of the GPD.
	lo_out.push_back(u32_get_byte0(gpdSecurityFrameCounter));
	lo_out.push_back(u32_get_byte1(gpdSecurityFrameCounter));
	lo_out.push_back(u32_get_byte2(gpdSecurityFrameCounter));
	lo_out.push_back(u32_get_byte3(gpdSecurityFrameCounter));
	// The forwarding radius.
	lo_out.push_back(forwardingRadius);

	return lo_out;
}

std::string CProcessGpPairingParam::toString() const {
	std::stringstream buf;

	buf << "ProcessGpPairingParam: { ";
	buf << "[options: 0x"<< std::hex << std::setw(8) << std::setfill('0') << static_cast<unsigned int>(this->options.get()) << "]";
	buf << "[addr: " << this->addr << "]";
	buf << "[commMode: 0x"<< std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(this->commMode) << "]";
	buf << "[sinkNetworkAddress: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(this->sinkNetworkAddress) << "]";
	buf << "[sinkGroupId: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(this->sinkGroupId) << "]";
	buf << "[assignedAlias: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(this->assignedAlias) << "]";
	buf << "[sinkIeeeAddress: " << NSSPI::Logger::byteSequenceToString(this->sinkIeeeAddress) << "]";
	buf << "[gpdKey: " << NSSPI::Logger::byteSequenceToString(this->gpdKey) << "]";
	buf << "[gpdSecurityFrameCounter: 0x"<< std::hex << std::setw(8) << std::setfill('0') << static_cast<unsigned int>(this->gpdSecurityFrameCounter) << "]";
	buf << "[forwardingRadius: 0x"<< std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(this->forwardingRadius) << "]";
	buf << " }";

	return buf.str();
}