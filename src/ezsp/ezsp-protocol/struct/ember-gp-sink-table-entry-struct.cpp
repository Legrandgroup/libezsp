/**
 * @file ember-gp-sink-table-entry-struct.cpp
 *
 * @brief Represents one sink table entry from Ember
 */

#include <sstream>
#include <iomanip>

#include "ezsp/ezsp-protocol/struct/ember-gp-sink-table-entry-struct.h"
#include "ezsp/byte-manip.h"

using NSEZSP::CEmberGpSinkTableEntryStruct;

CEmberGpSinkTableEntryStruct::CEmberGpSinkTableEntryStruct():
        status(0xFF),
        options(),
        gpd(),
        device_id(),
        sink_list(),
        assigned_alias(),
        groupcast_radius(),
        security_options(),
        gpdSecurity_frame_counter(),
        gpd_key()
{
    sink_list[0].push_back(0xFF);
    sink_list[1].push_back(0xFF);
}

CEmberGpSinkTableEntryStruct::~CEmberGpSinkTableEntryStruct()
{
}

CEmberGpSinkTableEntryStruct::CEmberGpSinkTableEntryStruct(const NSSPI::ByteBuffer& raw_message):
        status(raw_message.at(0)),
        options(dble_u8_to_u16(raw_message.at(2),raw_message.at(1))),
        gpd(NSSPI::ByteBuffer(raw_message.begin()+3,raw_message.end())),
        device_id(raw_message.at(13)),
        sink_list(),
        assigned_alias(dble_u8_to_u16(raw_message.at(37),raw_message.at(36))),
        groupcast_radius(raw_message.at(38)),
        security_options(raw_message.at(39)),
        gpdSecurity_frame_counter(static_cast<EmberGpSecurityFrameCounter>(quad_u8_to_u32(raw_message.at(43),raw_message.at(42),raw_message.at(41),raw_message.at(40)))),
        gpd_key(raw_message.begin()+44,raw_message.begin()+60)
{
    sink_list[0].push_back(0xFF);
    sink_list[1].push_back(0xFF);
}

CEmberGpSinkTableEntryStruct::CEmberGpSinkTableEntryStruct(EmberGpSinkTableEntryStatus i_status, CEmberGpSinkTableOption i_options,
                CEmberGpAddressStruct i_gpd_address, uint8_t i_device_id, uint16_t i_alias,
                uint8_t i_security_option, EmberGpSecurityFrameCounter i_frm_counter, EmberKeyData i_gpd_key):
        status(i_status),
        options(i_options),
        gpd(i_gpd_address),
        device_id(i_device_id),
        sink_list(),
        assigned_alias(i_alias),
        groupcast_radius(0),
        security_options(i_security_option),
        gpdSecurity_frame_counter(i_frm_counter),
        gpd_key(i_gpd_key)
{
    sink_list[0].push_back(0xFF);
    sink_list[1].push_back(0xFF);
}

NSSPI::ByteBuffer CEmberGpSinkTableEntryStruct::getRaw() const
{
    NSSPI::ByteBuffer l_struct;

    // Internal status of the sink table entry.
    l_struct.push_back(status); // 0x01 : active, 0xff : disable
    // The tunneling options (this contains both options and extendedOptions from the spec). WARNING 16 bits !!!
    l_struct.push_back(u16_get_lo_u8(options.get()));
    l_struct.push_back(u16_get_hi_u8(options.get()));
    // The addressing info of the GPD.
    NSSPI::ByteBuffer l_addr = gpd.getRaw();
    l_struct.insert(l_struct.end(), l_addr.begin(), l_addr.end());
    // The device id for the GPD.
    l_struct.push_back(device_id);
    // The list of sinks (hardcoded to 2 which is the spec minimum).
    for( int loop=0; loop<GP_SINK_LIST_ENTRIES; loop++ )
    {
        l_struct.insert(l_struct.end(), sink_list[loop].begin(), sink_list[loop].end());
    }
    // The assigned alias for the GPD.
    l_struct.push_back(u16_get_lo_u8(assigned_alias));
    l_struct.push_back(u16_get_hi_u8(assigned_alias));
    // The groupcast radius.
    l_struct.push_back(groupcast_radius);
    // The security options field.
    l_struct.push_back(security_options);
    // The security frame counter of the GPD.
    l_struct.push_back(u32_get_byte0(gpdSecurity_frame_counter));
    l_struct.push_back(u32_get_byte1(gpdSecurity_frame_counter));
    l_struct.push_back(u32_get_byte2(gpdSecurity_frame_counter));
    l_struct.push_back(u32_get_byte3(gpdSecurity_frame_counter));

    // The key to use for GPD.
    l_struct.insert(l_struct.end(), gpd_key.begin(), gpd_key.end());

    return l_struct;
}


std::string CEmberGpSinkTableEntryStruct::String() const
{
    std::stringstream buf;

    buf << "CEmberGpSinkTableEntryStruct : { ";
    buf << "[status : "<< std::hex << std::setw(2) << std::setfill('0') << unsigned(status) << "]";
    buf << "[options : "<< options << "]";
    buf << "[gpd : "<< gpd << "]";
    buf << "[device_id : "<< std::hex << std::setw(2) << std::setfill('0') << unsigned(device_id) << "]";
    buf << "[sink_list[0] :";
    for(uint8_t loop=0; loop<sink_list[0].size(); loop++){ buf << " " << std::hex << std::setw(2) << std::setfill('0') << unsigned(sink_list[0][loop]); }
    buf << "]";
    buf << "[sink_list[1] :";
    for(uint8_t loop=0; loop<sink_list[1].size(); loop++){ buf << " " << std::hex << std::setw(2) << std::setfill('0') << unsigned(sink_list[1][loop]); }
    buf << "]";
    buf << "[assigned_alias : "<< std::hex << std::setw(4) << std::setfill('0') << unsigned(assigned_alias) << "]";
    buf << "[groupcast_radius : "<< std::hex << std::setw(2) << std::setfill('0') << unsigned(groupcast_radius) << "]";
    buf << "[security_options : "<< std::hex << std::setw(2) << std::setfill('0') << unsigned(security_options) << "]";
    buf << "[gpdSecurity_frame_counter : "<< std::hex << std::setw(8) << std::setfill('0') << unsigned(gpdSecurity_frame_counter) << "]";
    buf << "[gpd_key :";
    for(uint8_t loop=0; loop<gpd_key.size(); loop++){ buf << " " << std::hex << std::setw(2) << std::setfill('0') << unsigned(gpd_key[loop]); }
    buf << "]";
    buf << " }";

    return buf.str();
}
