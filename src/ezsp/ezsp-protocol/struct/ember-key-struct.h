/**
 * @file ember-key-struct.h
 *
 * @brief A structure containing a key and its associated data.
 */

#pragma once

#include "ezsp/ezsp-protocol/ezsp-enum.h"
#include "spi/ByteBuffer.h"

namespace NSEZSP {

class CEmberKeyStruct
{
    public:
        /**
         * @brief Default constructor
         *
         * Construction without arguments is not allowed
         */
        CEmberKeyStruct() = delete;

        /**
         * @brief Construction from a buffer
         *
         * @param raw_message The buffer to construct from
         */
        explicit CEmberKeyStruct(const NSSPI::ByteBuffer& raw_message);

        /**
         * @brief Copy constructor
         *
         * Copy construction is forbidden on this class
         */
        CEmberKeyStruct(const CEmberKeyStruct& other) = delete;

        /**
         * @brief Assignment operator
         *
         * Assignment is forbidden on this class
         */
        CEmberKeyStruct& operator=(const CEmberKeyStruct& other) = delete;

        /**
         * @brief A bitmask indicating the presence of data within the various fields in the structure.
         */
        EmberKeyStructBitmask getBitmask() const { return bitmask; }

        /**
         * @brief The type of the key.
         */
        EmberKeyType getType() const { return type; }

        /**
         * @brief The actual key data.
         */
        EmberKeyData getKey() const { return key; }

        /**
         * @brief The outgoing frame counter associated with the key.
         */
        uint32_t getOutgoingFrameCounter() const { return outgoingFrameCounter; }

        /**
         * The frame counter of the partner device associated
         * with the key.
         */
        uint32_t getIncomingFrameCounter() const { return incomingFrameCounter; }

        /**
         * @brief The sequence number associated with the key.
         */
        uint8_t getSequenceNumber() const { return sequenceNumber; }

        /**
         * @brief The IEEE address of the partner device also in possession of the key.
         */
        EmberEUI64 getPartnerEUI64() const { return partnerEUI64; }

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
        friend std::ostream& operator<< (std::ostream& out, const CEmberKeyStruct& data){
            out << data.String();
            return out;
        }

    private:
        EmberKeyStructBitmask bitmask;
        EmberKeyType type;
        EmberKeyData key;
        uint32_t outgoingFrameCounter;
        uint32_t incomingFrameCounter;
        uint8_t sequenceNumber;
        EmberEUI64 partnerEUI64;
};

} // namespace NSEZSP
