/**
 * @file gpd-commissionning-command-payload.h
 *
 * @brief decoding payload of gpd commissioning command according to A.4.2.1.1 GPD Commissioning command from docs-14-0563-16-batt-green-power-spec_ProxyBasic.pdf
 */
#pragma once

#include <cstdint>
#include <vector>

#include "../ezsp-protocol/ezsp-enum.h"

// option bitfield
#define COM_OPTION_MAC_SEQ_CAPABILITY_BIT       0
#define COM_OPTION_RX_ON_CAPABILITY_BIT         1
#define COM_OPTION_APPLICATION_INFORMATION_BIT  2
#define COM_OPTION_PAN_ID_REQUEST_BIT           4
#define COM_OPTION_GP_SECURITY_KEY_REQUEST_BIT  5
#define COM_OPTION_FIXED_LOCATION_BIT           6
#define COM_OPTION_EXTENDED_OPTION_FIELD_BIT    7

// extend option bitfield
#define COM_EXT_OPTION_SECURITY_LVL_CAPABILITY_BIT  0
#define COM_EXT_OPTION_KEY_TYPE_BIT                 2
#define COM_EXT_OPTION_GPD_KEY_PRESENT_BIT          5
#define COM_EXT_OPTION_GPD_KEY_ENCRYPTION_BIT       6
#define COM_EXT_OPTION_GPD_OUT_COUNTER_PRESENT_BIT  7

// application information bitfield
#define COM_APP_INFO_MANUFACTURER_ID_PRESENT_BIT    0
#define COM_APP_INFO_MODEL_ID_PRESENT_BIT           1
#define COM_APP_INFO_GPD_COMMANDS_PRESENT_BIT       2
#define COM_APP_INFO_CLUSTER_LIST_PRESENT_BIT       3

class CGpdCommissioningPayload
{
    public:

        /**
         * @brief Default constructor
         *
         * Construction without arguments is not allowed
         */
        CGpdCommissioningPayload() = delete;

        /**
         * @brief Assignment operator
         *
         * Copy construction is forbidden on this class
         */
        CGpdCommissioningPayload& operator=(const CGpdCommissioningPayload& other) = delete;

        /**
         * @brief Construction from an incoming ezsp raw message
         *
         * @param raw_message The buffer to construct from
         * @param i_src_id source id of gpd frame, use to decrypt key
         */
        CGpdCommissioningPayload(const std::vector<uint8_t>& raw_message, uint32_t i_src_id);

        /**
         * @brief Getter key
         * 
         * @return key
         */
        EmberKeyData getKey(){ return key; }

        /**
         * @brief Getter
         * 
         * @return deviceId
         */
        uint8_t getDeviceId(){ return device_id; }

        // bits field:
        // b0 : MACsequenceNumberCapability (0b1:incremental MAC sequence number, 0b0:random MAC sequence number)
        // b1 : RxOnCapability (0b1:GPD has receiving capabilities in operational mode.)
        // b2 : Application information present
        // b3 : reserved
        // b4 : PANId request
        // b5 : GP Security Key request
        // b6 : FixedLocation
        // b7 : Extended Options Field
        /**
         * @brief options getters bit field
         */
        bool isMACsequenceNumberCapability(){ return (options & (1<<COM_OPTION_MAC_SEQ_CAPABILITY_BIT)); }
        bool isRxOnCapability(){ return (options & (1<<COM_OPTION_RX_ON_CAPABILITY_BIT)); }
        bool isApplicationInformationPresent(){ return (options & (1<<COM_OPTION_APPLICATION_INFORMATION_BIT)); }
        bool isPANIdRequest(){ return (options & (1<<COM_OPTION_PAN_ID_REQUEST_BIT)); }
        bool isGPSecurityKeyRequest(){ return (options & (1<<COM_OPTION_GP_SECURITY_KEY_REQUEST_BIT)); }
        bool isFixedLocation(){ return (options & (1<<COM_OPTION_FIXED_LOCATION_BIT)); }
        bool isExtendedOptionsFieldPresent(){ return (options & (1<<COM_OPTION_EXTENDED_OPTION_FIELD_BIT)); }

        /**
         * @brief Getter
         * 
         * @return extended_options
         */
        uint8_t getExtendedOption(){ return extended_options; }

        /**
         * @brief Getter
         * 
         * @return out_frame_counter
         */
        uint32_t getOutFrameCounter(){ return out_frame_counter; }


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
        friend std::ostream& operator<< (std::ostream& out, const CGpdCommissioningPayload& data);

    private:
        // define in ZigBee document 13-0166, Master List of Green Power Device Definitions, revision 00 or later
        uint8_t device_id;
        // bits field:
        // b0 : MACsequenceNumberCapability (0b1:incremental MAC sequence number, 0b0:random MAC sequence number)
        // b1 : RxOnCapability (0b1:GPD has receiving capabilities in operational mode.)
        // b2 : Application information present
        // b3 : reserved
        // b4 : PANId request
        // b5 : GP Security Key request
        // b6 : FixedLocation
        // b7 : Extended Options Field
        uint8_t options;
        // bits field:
        // b0-1 : SecurityLevelCapabilities (0b00:No security, 0b01:Reserved, 0b10:4B frame counter and 4B MIC Only, 0b11:Encryption & 4B frame counter and 4B MIC)
        // b2-4 : KeyType : (see A.3.7.1.2 gpSecurityKeyType)
        //          0b000 : no key
        //          0b001 : ZigBee NWK key
        //          0b010 : GPD group key
        //          0b011 : NWK-key derived GPD group key
        //          0b100 : (individual) outof-the-box GPD key
        //          0b101-0b110 : Reserved
        //          0b111 : Derived individual GPD key
        // b5 : GPD Key present
        // b6 : GPD Key encryption
        // b7 : GPD outgoing counter present
        uint8_t extended_options;

        EmberKeyData key;
        uint32_t key_mic;
        uint32_t out_frame_counter;
        // bits field:
        // b0 : ManufacturerID present
        // b1 : ModelID present
        // b2 : GPD commands present
        // b3 : Cluster list present
        // b4-7 : reserved
        uint8_t app_information;
        uint16_t manufacturer_id;
        uint16_t model_id;
        std::vector<uint8_t> gpd_command_list;
        std::vector<uint8_t> gpd_cluster_list;
};