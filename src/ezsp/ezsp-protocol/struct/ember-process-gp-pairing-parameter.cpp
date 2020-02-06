/**
 * @file ember-process-gp-pairing-parameter.cpp
 *
 * @brief Parameters for gpProxyTableProcessGpPairing (0xC9) command.
 *
 * Reference: docs-14-0563-16-batt-green-power-spec_ProxyBasic.pdf
 */

#include "ezsp/byte-manip.h"
#include "ember-process-gp-pairing-parameter.h"

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
    lo_out.push_back(u32_get_byte0(l_option));
    lo_out.push_back(u32_get_byte1(l_option));
    lo_out.push_back(u32_get_byte2(l_option));
    lo_out.push_back(u32_get_byte3(l_option));
    // The addressing info of the target GPD.
    std::vector<uint8_t> l_gpd_addr = addr.getRaw();
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
