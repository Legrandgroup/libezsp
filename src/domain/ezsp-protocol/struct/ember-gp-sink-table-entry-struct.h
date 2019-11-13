/**
 * @file ember-gp-sink-table-entry-struct.h
 *
 * @brief The internal representation of a sink table entry.
 */

#pragma once

#include "../ezsp-enum.h"
#include "ember-gp-address-struct.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

class CEmberGpSinkTableEntryStruct
{
    public:
        /**
         * @brief Default constructor
         *
         * Construction without arguments is not allowed
         */
        CEmberGpSinkTableEntryStruct() = delete;

        /**
         * @brief Construction from a buffer
         *
         * @param raw_message The buffer to construct from
         */
        CEmberGpSinkTableEntryStruct(const std::vector<uint8_t>& raw_message);

        /**
         * @brief constructor with specific value, others are set to default value.
         *          option : 0x02A8 (cf. A3.3.2.2.1 Options parameter of the Sink Table from doc doc-14-0563-16-batt-green-power-spec_ProxyBasic.pdf)
         *              - bits 0..2 : Application Id (0b000 : use source Id)
         *              - bits 3..4 : Communication mode (0b01 : groupcast forwarding of the GP Notification command to DGroupID)
         *              - bit 5 : Sequence number capabilities (0b1 : use incremental sequence number)
         *              - bit 6 : RxOnCapability (0b0 : not capable)
         *              - bit 7 : FixedLocation (0b1 : not a mobile device)
         *              - bit 8 : AssignedAlias (0b0 : the derived alias is used)
         *              - bit 9 : Security use (0b1 : indicates that security-related parameters of the Sink Table entry are present)
         *              - bit 10..15 : Reserved
         *          sink_list : by reverse set first byte of each to 0xFF to disable usage.
         *          groupcast_radius : The default value of 0x00 indicates undefined
         * 
         * @param i_status Internal status of the sink table entry. 0x01 active, 0xff : disable
         * @param i_gpd_address The addressing info of the GPD.
         * @param i_device_id The device id for the GPD.
         * @param i_alias The assigned alias for the GPD.
         * @param i_security_option The security options field. currently 0x12 : SecurtityLevel (0b10), SecurityKeyType(0b100)
         * @param i_frm_counter the last observed valid frame counter value
         * @param i_gpd_key The key to use for GPD.
         */
        CEmberGpSinkTableEntryStruct(EmberGpSinkTableEntryStatus i_status, 
                        CEmberGpAddressStruct i_gpd_address, uint8_t i_device_id, uint16_t i_alias,
                        uint8_t i_security_option, EmberGpSecurityFrameCounter i_frm_counter, EmberKeyData i_gpd_key);        

        /**
         * @brief Copy constructor
         *
         * Copy construction is forbidden on this class
         */
        CEmberGpSinkTableEntryStruct(const CEmberGpSinkTableEntryStruct& other) = delete;

        /**
         * @brief Assignment operator
         *
         * Assignment is forbidden on this class
         */
        CEmberGpSinkTableEntryStruct& operator=(const CEmberGpSinkTableEntryStruct& other) = delete;

        /**
         * @brief return structure as a raw
         * 
         * @return raw of structure
         */
        std::vector<uint8_t> getRaw(void);


    private:
        // Internal status of the sink table entry.
        EmberGpSinkTableEntryStatus status;
        // The tunneling options (this contains both options and extendedOptions from the spec). WRONG Specification only 16 bits like option without extended ...
        uint16_t options;
        // The addressing info of the GPD.
        CEmberGpAddressStruct gpd;
        // The device id for the GPD.
        uint8_t device_id;
        // The list of sinks (hardcoded to 2 which is the spec minimum).
        EmberGpSinkListEntry sink_list[GP_SINK_LIST_ENTRIES];
        // The assigned alias for the GPD.
        EmberNodeId assigned_alias;
        // The groupcast radius.
        uint8_t groupcast_radius;
        // The security options field.
        uint8_t security_options;
        // The security frame counter of the GPD.
        EmberGpSecurityFrameCounter gpdSecurity_frame_counter;
        // The key to use for GPD.
        EmberKeyData gpd_key;
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN