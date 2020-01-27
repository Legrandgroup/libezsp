/**
 * @file ember-process-gp-pairing-parameter.cpp
 *
 * @brief Parameters for gpProxyTableProcessGpPairing (0xC9) command.
 *
 * Reference: docs-14-0563-16-batt-green-power-spec_ProxyBasic.pdf
 */

#include "ember-process-gp-pairing-parameter.h"

using NSEZSP::CProcessGpPairingParam;

CProcessGpPairingParam::CProcessGpPairingParam(uint32_t i_gpd_source_id) :
        options(0, false, true, 0, 0, false, false, false),
        addr(i_gpd_source_id),
        commMode(),
        sinkNetworkAddress(),
        sinkGroupId(),
        assignedAlias(),
        sinkIeeeAddress(),
        gpdKey(),
        gpdSecurityFrameCounter(),
        forwardingRadius()
{

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
        forwardingRadius(i_sink_table_entry.getGroupcastRadius())
{

}


std::vector<uint8_t> CProcessGpPairingParam::get() const
{
    std::vector<uint8_t> lo_out;

    // The options field of the GP Pairing command
    uint32_t l_option = options.get();
    lo_out.push_back(static_cast<uint8_t>(l_option&0xFF));
    lo_out.push_back(static_cast<uint8_t>((l_option>>8)&0xFF));
    lo_out.push_back(static_cast<uint8_t>((l_option>>16)&0xFF));
    lo_out.push_back(static_cast<uint8_t>((l_option>>24)&0xFF));
    // The addressing info of the target GPD.
    std::vector<uint8_t> l_gpd_addr = addr.getRaw();
    lo_out.insert(lo_out.end(),l_gpd_addr.begin(),l_gpd_addr.end());
    // The communication mode of the GP Sink.
    lo_out.push_back(commMode);
    // The network address of the GP Sink.
    lo_out.push_back(static_cast<uint8_t>(sinkNetworkAddress&0xFF));
    lo_out.push_back(static_cast<uint8_t>(static_cast<uint8_t>(sinkNetworkAddress>>8)&0xFF));
    // The group ID of the GP Sink.
    lo_out.push_back(static_cast<uint8_t>(sinkGroupId&0xFF));
    lo_out.push_back(static_cast<uint8_t>(static_cast<uint8_t>(sinkGroupId>>8)&0xFF));
    // The alias assigned to the GPD.
    lo_out.push_back(static_cast<uint8_t>(assignedAlias&0xFF));
    lo_out.push_back(static_cast<uint8_t>(static_cast<uint8_t>(assignedAlias>>8)&0xFF));
    // The IEEE address of the GP Sink.
    lo_out.insert(lo_out.end(),sinkIeeeAddress.begin(),sinkIeeeAddress.end());
    // The key to use for GPD.
    lo_out.insert(lo_out.end(),gpdKey.begin(),gpdKey.end());
    // The security frame counter of the GPD.
    lo_out.push_back(static_cast<uint8_t>(gpdSecurityFrameCounter&0xFF));
    lo_out.push_back(static_cast<uint8_t>(static_cast<uint8_t>(gpdSecurityFrameCounter>>8)&0xFF));
    lo_out.push_back(static_cast<uint8_t>(static_cast<uint8_t>(gpdSecurityFrameCounter>>16)&0xFF));
    lo_out.push_back(static_cast<uint8_t>(static_cast<uint8_t>(gpdSecurityFrameCounter>>24)&0xFF));
    // The forwarding radius.
    lo_out.push_back(forwardingRadius);

    return lo_out;
}
