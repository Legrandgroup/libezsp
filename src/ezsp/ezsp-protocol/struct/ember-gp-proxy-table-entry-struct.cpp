/**
 * @file ember-gp-proxy-table-entry-struct.cpp
 *
 * @brief Represents one sink table entry from Ember
 */

#include <sstream>
#include <iomanip>

#include "ezsp/ezsp-protocol/struct/ember-gp-proxy-table-entry-struct.h"

#include "ezsp/byte-manip.h"

/** \todo Verify value !!! */
CEmberGpProxyTableEntryStruct::CEmberGpProxyTableEntryStruct(const std::vector<uint8_t>& raw_message) :
        /*security_link_key(raw_message.begin(),raw_message.begin()+EMBER_KEY_DATA_BYTE_SIZE),*/
        status(raw_message.at(0)),
        options(quad_u8_to_u32(raw_message.at(4),raw_message.at(3),raw_message.at(2),raw_message.at(1))),
        gpd(std::vector<uint8_t>(raw_message.begin()+5,raw_message.end())),
        assigned_alias(dble_u8_to_u16(raw_message.at(16),raw_message.at(15))),
        security_options(raw_message.at(17)),
        gpdSecurityFrameCounter(static_cast<EmberGpSecurityFrameCounter>(quad_u8_to_u32(raw_message.at(21),raw_message.at(20),raw_message.at(19),raw_message.at(18)))),
        gpd_key(raw_message.begin()+22,raw_message.begin()+22+EMBER_KEY_DATA_BYTE_SIZE),
        sink_list(),
        groupcast_radius(raw_message.at(60)),
        search_counter(raw_message.at(61))
{

}
