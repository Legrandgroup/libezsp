/**
 * @file gp-pairing-command-option-struct.cpp
 *
 * @brief option field of gp pairing command according to A.3.3.5.2 GP Pairing command from docs-14-0563-16-batt-green-power-spec_ProxyBasic.pdf
 */

#include "gp-pairing-command-option-struct.h"

CGpPairingCommandOption::CGpPairingCommandOption(CEmberGpSinkTableOption i_sink_table_option, bool i_add_sink, bool i_remove_gpd,
                                                    uint8_t i_security_level, uint8_t i_security_key_type, bool i_frm_counter_present,
                                                    bool i_key_present, bool i_radius_present ):
        application_id(i_sink_table_option.getApplicationId()),
        add_sink(i_add_sink),
        remove_gpd(i_remove_gpd),
        communication_mode(i_sink_table_option.getCommunicationMode()),
        gpd_fixed(i_sink_table_option.isFixedLocation()),
        gpd_mac_seq_number_capability(i_sink_table_option.isSequenceNumberCapabilities()),
        security_level(i_security_level),
        security_key_type(i_security_key_type),
        gpd_security_frame_counter_present(i_frm_counter_present),
        gpd_security_key_present(i_key_present),
        assigned_alias_present(i_sink_table_option.isAssignedAlias()),
        forwarding_radius_present(i_radius_present)
{

}

uint32_t CGpPairingCommandOption::get() const
{
        uint32_t lo_option;

        lo_option = static_cast<uint32_t>(application_id<<0);
        lo_option |= static_cast<uint32_t>(add_sink<<3);
        lo_option |= static_cast<uint32_t>(remove_gpd<<4);
        lo_option |= static_cast<uint32_t>(communication_mode<<5);
        lo_option |= static_cast<uint32_t>(gpd_fixed<<7);
        lo_option |= static_cast<uint32_t>(gpd_mac_seq_number_capability<<8);
        lo_option |= static_cast<uint32_t>(security_level<<9);
        lo_option |= static_cast<uint32_t>(security_key_type<<11);
        lo_option |= static_cast<uint32_t>(gpd_security_frame_counter_present<<14);
        lo_option |= static_cast<uint32_t>(gpd_security_key_present<<15);
        lo_option |= static_cast<uint32_t>(assigned_alias_present<<16);
        lo_option |= static_cast<uint32_t>(forwarding_radius_present<<17);

        return lo_option;
}
