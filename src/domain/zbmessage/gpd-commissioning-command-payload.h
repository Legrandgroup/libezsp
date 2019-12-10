/**
 * @file gpd-commissioning-command-payload.h
 *
 * @brief Decoding payload of gpd commissioning command according to A.4.2.1.1 GPD Commissioning command from docs-14-0563-16-batt-green-power-spec_ProxyBasic.pdf
 */
#pragma once

#include <cstdint>
#include <vector>

#include "domain/ezsp-protocol/ezsp-enum.h"

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
         * Assignment is forbidden on this class
         */
        CGpdCommissioningPayload& operator=(const CGpdCommissioningPayload& other) = delete;

        /**
         * @brief Construction from an incoming ezsp raw message
         *
         * @param raw_message The buffer to construct from
         * @param i_src_id source id of gpd frame, used to decrypt key
         */
        CGpdCommissioningPayload(const std::vector<uint8_t>& raw_message, uint32_t i_src_id);

        /**
         * @brief Getter for the enclosed encryption/authentication key
         * 
         * @return The enclosed key
         */
        EmberKeyData getKey() const { return key; }

        /**
         * @brief Getter for the enclosed device ID
         * 
         * @return The enclosed deviceId
         */
        uint8_t getDeviceId() const { return device_id; }

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
        bool isMACsequenceNumberCapability() const { return (options & (1<<COM_OPTION_MAC_SEQ_CAPABILITY_BIT)); }
        bool isRxOnCapability() const { return (options & (1<<COM_OPTION_RX_ON_CAPABILITY_BIT)); }
        bool isApplicationInformationPresent() const { return (options & (1<<COM_OPTION_APPLICATION_INFORMATION_BIT)); }
        bool isPANIdRequest() const { return (options & (1<<COM_OPTION_PAN_ID_REQUEST_BIT)); }
        bool isGPSecurityKeyRequest() const { return (options & (1<<COM_OPTION_GP_SECURITY_KEY_REQUEST_BIT)); }
        bool isFixedLocation() const { return (options & (1<<COM_OPTION_FIXED_LOCATION_BIT)); }
        bool isExtendedOptionsFieldPresent() const { return (options & (1<<COM_OPTION_EXTENDED_OPTION_FIELD_BIT)); }

        /**
         * @brief Getter for the enclosed extended options
         * 
         * @return The enclosed extended options
         */
        uint8_t getExtendedOption() const { return extended_options; }

        /**
         * @brief Getter for the enclosed frame counter
         * 
         * @return The enclosed frame counter
         */
        uint32_t getOutFrameCounter() const { return out_frame_counter; }


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
        uint8_t device_id; /*!< The device ID contained in this GPD commissioning command */
        // bits field:
        // b0 : MACsequenceNumberCapability (0b1:incremental MAC sequence number, 0b0:random MAC sequence number)
        // b1 : RxOnCapability (0b1:GPD has receiving capabilities in operational mode.)
        // b2 : Application information present
        // b3 : reserved
        // b4 : PANId request
        // b5 : GP Security Key request
        // b6 : FixedLocation
        // b7 : Extended Options Field
        uint8_t options; /*!< The options field contained in this GPD commissioning command */
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
        uint8_t extended_options; /*!< The extended options contained in this GPD commissioning command */

        EmberKeyData key; /*!< The key contained in this GPD commissioning command */
        uint32_t key_mic; /*!< The MIC contained in this GPD commissioning command */
        uint32_t out_frame_counter; /*!< The frame counter value contained in this GPD commissioning command */
        // bits field:
        // b0 : ManufacturerID present
        // b1 : ModelID present
        // b2 : GPD commands present
        // b3 : Cluster list present
        // b4-7 : reserved
        uint8_t app_information; /*!< The app information contained in this GPD commissioning command */
        uint16_t manufacturer_id; /*!< The manufacturer ID contained in this GPD commissioning command */
        uint16_t model_id; /*!< The model ID contained in this GPD commissioning command */
        std::vector<uint8_t> gpd_command_list; /*!< The GPD command list contained in this GPD commissioning command */
        std::vector<uint8_t> gpd_cluster_list; /*!< The GPD cluster list contained in this GPD commissioning command */
};
