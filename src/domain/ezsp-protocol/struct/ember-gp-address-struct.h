/**
 * @file ember-gp-address-struct.h
 *
 * @brief A GP address structure.
 */
#pragma once

#include "../ezsp-enum.h"

class CEmberGpAddressStruct
{
    public:
        /**
         * @brief Default constructor
         *
         * Construction without arguments is not allowed
         */
        CEmberGpAddressStruct() = delete;

        /**
         * @brief Construction from a buffer
         *
         * @param raw_message The buffer to construct from
         */
        CEmberGpAddressStruct(const std::vector<uint8_t>& raw_message);

        /**
         * @brief Assignment operator
         *
         * Copy construction is forbidden on this class
         */
        CEmberGpAddressStruct& operator=(const CEmberGpAddressStruct& other) = delete;

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
        friend std::ostream& operator<< (std::ostream& out, const CEmberGpAddressStruct& data);          

    private:

        EmberEUI64 gpdIeeeAddress; // The GPD's EUI64.
        uint32_t sourceId; // The GPD's source ID.
        uint8_t applicationId; // The GPD Application ID.
        uint8_t endpoint; // The GPD endpoint.

};
