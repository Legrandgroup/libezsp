/**
 * @file green-power-frame.h
 *
 * @brief Handles decoding of a green power frame
 */
#pragma once

#include <cstdint>
#include <vector>

typedef enum
{
    GPD_NO_SECURITY                      =       0x0,
    GPD_FRM_COUNTER_MIC_SECURITY         =       0x2,
    GPD_ENCRYPT_FRM_COUNTER_MIC_SECURITY =       0x3
}EGpSecurityLevel;

typedef enum
{
    GPD_KEY_TYPE_NO_KEY                         =       0x0,
    GPD_KEY_TYPE_ZB_NWK_KEY                     =       0x1,
    GPD_KEY_TYPE_GPD_GROUP_KEY                  =       0x2,
    GPD_KEY_TYPE_NWK_KEY_DERIVED_GPD_GROUP_KEY  =       0x3,
    GPD_KEY_TYPE_OOB_KEY                        =       0x4,
    GPD_KEY_TYPE_DERIVED_INDIVIDUAL_KEY         =       0x7,
}EGpSecurityKeyType;

class CGpFrame
{
    public:
        /**
         * @brief Default constructor
         *
         * Construction without arguments is not allowed
         */
        CGpFrame() = delete;

        /**
         * @brief Assignment operator
         *
         * Copy construction is forbidden on this class
         */
        CGpFrame& operator=(const CGpFrame& other) = delete;

        /**
         * @brief Construction from an incoming ezsp raw message
         *
         * @param raw_message The buffer to construct from
         */
        CGpFrame(const std::vector<uint8_t>& raw_message);

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
        friend std::ostream& operator<< (std::ostream& out, const CGpFrame& data);

        // getter
        uint8_t getLinkValue(){return link_value;}
        uint8_t getSequenceNumber(){return sequence_number;}
        uint32_t getSourceId(){return source_id;}
        EGpSecurityLevel getSecurity(){return security;}
        EGpSecurityKeyType getKeyType(){return key_type;}
        bool isAutoCommissioning(){return auto_commissioning;}
        bool isRxAfterTx(){return rx_after_tx;}
        uint32_t getSecurityFrameCounter(){return security_frame_counter;}
        uint8_t getCommandId(){return command_id;}
        uint32_t getMic(){return mic;}
        uint8_t getProxyTableEntry(){return proxy_table_entry;}
        std::vector<uint8_t> getPayload(){return payload;}


    private:
        uint8_t link_value;
        uint8_t sequence_number;
        uint32_t source_id;
        EGpSecurityLevel security;
        EGpSecurityKeyType key_type;
        bool auto_commissioning;
        bool rx_after_tx;
        uint32_t security_frame_counter;
        uint8_t command_id;
        uint32_t mic;
        uint8_t proxy_table_entry;
        std::vector<uint8_t> payload;
};