/**
 * @file ember-child-data-struct.h
 *
 * @brief A structure containing a child node's data.
 */

#pragma once

#include "ezsp/ezsp-protocol/ezsp-enum.h"
#include "spi/ByteBuffer.h"

namespace NSEZSP {

class CEmberChildDataStruct
{
    public:
        /**
         * @brief Default constructor
         *
         * Construction without arguments is not allowed
         */
        CEmberChildDataStruct() = delete;

        /**
         * @brief Construction from a buffer
         *
         * @param raw_message The buffer to construct from
         */
        explicit CEmberChildDataStruct(const NSSPI::ByteBuffer& raw_message);

        /**
         * @brief Assignment operator
         *
         * Copy construction is forbidden on this class
         */
        CEmberChildDataStruct& operator=(const CEmberChildDataStruct& other) = delete;

        /**
         * @brief The EUI64 of the child
         */
        EmberEUI64 getEui64() const { return eui64; }

        /**
         * @brief The node type of the child
         */
        EmberNodeType getType() const { return type; }

        /**
         * @brief The short address of the child
         */
        EmberNodeId getId() const { return id; }

        /**
         * @brief The phy of the child
         */
        uint8_t getPhy() const { return phy; }

        /**
         * @brief The power of the child
         */
        uint8_t getPower() const { return power; }

        /**
         * The timeout of the child
         */
        uint8_t getTimeout() const { return timeout; }

        /**
         * @brief The GPD's EUI64.
         */
        EmberEUI64 getGpdIeeeAddress() const { return gpdIeeeAddress; }

        /**
         * @brief The GPD's source ID.
         */
        uint32_t getSourceId() const { return sourceId; }

        /**
         * @brief The GPD Application ID.
         */
        uint8_t getApplicationId() const { return applicationId; }

        /**
         * @brief The GPD endpoint.
         */
        uint8_t getEndpoint() const { return endpoint; }

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
        friend std::ostream& operator<< (std::ostream& out, const CEmberChildDataStruct& data){
            out << data.String();
            return out;
        }

    private:
        EmberEUI64 eui64; // The EUI64 of the child
        EmberNodeType type; // The node type of the child
        EmberNodeId id; // The short address of the child
        uint8_t phy; // The phy of the child
        uint8_t power; // The power of the child
        uint8_t timeout; // The timeout of the child
        EmberEUI64 gpdIeeeAddress; // The GPD's EUI64.
        uint32_t sourceId; // The GPD's source ID.
        uint8_t applicationId; // The GPD Application ID.
        uint8_t endpoint; // The GPD endpoint.
};

} // namespace NSEZSP
