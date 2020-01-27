/**
 * @file ember-gp-sink-table-entry-struct.h
 *
 * @brief Represents one sink table entry from Ember
 */

#pragma once

#include "ezsp/ezsp-protocol/ezsp-enum.h"
#include "ezsp/ezsp-protocol/struct/ember-gp-address-struct.h"
#include "ezsp/ezsp-protocol/struct/ember-gp-sink-table-options-field.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

namespace NSEZSP {

class CEmberGpSinkTableEntryStruct
{
    public:
        /**
         * @brief Default constructor
         */
        CEmberGpSinkTableEntryStruct();

        /**
         * @brief Construction from a buffer
         *
         * @param raw_message The buffer to construct from
         */
        explicit CEmberGpSinkTableEntryStruct(const std::vector<uint8_t>& raw_message);

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
         * @param i_option see upper
         * @param i_gpd_address The addressing info of the GPD.
         * @param i_device_id The device id for the GPD.
         * @param i_alias The assigned alias for the GPD.
         * @param i_security_option The security options field. currently 0x12 : SecurtityLevel (0b10), SecurityKeyType(0b100)
         * @param i_frm_counter the last observed valid frame counter value
         * @param i_gpd_key The key to use for GPD.
         */
        CEmberGpSinkTableEntryStruct(EmberGpSinkTableEntryStatus i_status, CEmberGpSinkTableOption i_options,
                        CEmberGpAddressStruct i_gpd_address, uint8_t i_device_id, uint16_t i_alias,
                        uint8_t i_security_option, EmberGpSecurityFrameCounter i_frm_counter, EmberKeyData i_gpd_key);

        /**
         * @brief return structure as a raw
         *
         * @return raw of structure
         */
        std::vector<uint8_t> getRaw() const;

        /**
         * @brief getters
         */
        CEmberGpSinkTableOption getOption() const { return options; }
        CEmberGpAddressStruct getGpdAddr() const { return gpd; }
        EmberNodeId getAssignedAlias() const { return assigned_alias; }
        EmberGpSecurityFrameCounter getSecurityFrameCounter() const { return gpdSecurity_frame_counter; }
        EmberKeyData getGpdKey() const { return gpd_key; }
        uint8_t getGroupcastRadius() const { return groupcast_radius; }
        uint8_t getSecurityLevel() const { return security_options&0x03; }
        uint8_t getSecurityKeyType() const { return (static_cast<uint8_t>(security_options>>2)&0x07); }
        bool isActive() const { return status==0x01; }

        /**
         * @brief status setter
         */
        void setEntryActive(bool i_active) { this->status=(i_active?0x01:0xFF); }

        /**
         * @brief options setter
         */
        void setOptions(CEmberGpSinkTableOption i_options){ options=i_options; }

        /**
         * @brief gpd setter
         */
        void setGpdAddress(CEmberGpAddressStruct i_gpd_address){ gpd=i_gpd_address; }

        /**
         * @brief device_id setter
         */
        void setDeviceId(uint8_t i_device_id){ device_id=i_device_id; }

        /**
         * @brief assigned_alias setter
         */
        void setAlias(uint16_t i_alias){ assigned_alias=i_alias; }

        /**
         * @brief security_options setter
         */
        void setSecurityOption(uint8_t i_security_option){ security_options=i_security_option; }

        /**
         * @brief gpdSecurity_frame_counter setter
         */
        void setFrameCounter(EmberGpSecurityFrameCounter i_frm_counter){ gpdSecurity_frame_counter=i_frm_counter; }

        /**
         * @brief gpd_key setter
         */
        void setKey(EmberKeyData i_gpd_key){ gpd_key=i_gpd_key; }

        /**
         * @brief Dump this instance as a string
         *
         * @return The resulting string
         */
        std::string String() const;

        /**
         * @brief Serialize to an iostream
         *
         * @param out The original output stream
         * @param data The object to serialize
         *
         * @return The new output stream with serialized data appended
         */
        friend std::ostream& operator<< (std::ostream& out, const CEmberGpSinkTableEntryStruct& data){
			out << data.String();
			return out;
		}

    private:
        EmberGpSinkTableEntryStatus status; /*!< Internal status of the sink table entry */
        CEmberGpSinkTableOption options; /*!< The tunneling options (this contains both options and extendedOptions from the spec). WRONG Specification only 16 bits like option without extended... */
        CEmberGpAddressStruct gpd; /*!< The addressing info of the GPD */
        uint8_t device_id; /*!< The device id for the GPD */
        EmberGpSinkListEntry sink_list[GP_SINK_LIST_ENTRIES]; /*!< The list of sinks (hardcoded to 2 which is the spec minimum) */
        EmberNodeId assigned_alias; /*!< The assigned alias for the GPD */
        uint8_t groupcast_radius; /*!< The groupcast radius */
        uint8_t security_options; /*!< The security options field */
        EmberGpSecurityFrameCounter gpdSecurity_frame_counter; /*!< The security frame counter of the GPD */
        EmberKeyData gpd_key; /*!< The key to use for GPD */
};

} // namespace NSEZSP

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
