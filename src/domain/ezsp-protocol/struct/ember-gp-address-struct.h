/**
 * @file ember-gp-address-struct.h
 *
 * @brief A GP address structure.
 */
#pragma once

#include "../ezsp-enum.h"
#include "../../byte-manip.h"

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
         * @brief Construction from itself
         *
         * @param i_gpd_address gpd address from construct
         */
        CEmberGpAddressStruct(const CEmberGpAddressStruct& i_gpd_address);

        /**
         * @brief Construction from a buffer
         *
         * @param raw_message The buffer to construct from
         */
        CEmberGpAddressStruct(const std::vector<uint8_t>& raw_message);

        /**
         * @brief Construct from sourceId
         * 
         * @param i_srcId SourceId to construct from
         */
        CEmberGpAddressStruct(const uint32_t i_srcId);

        /**
         * @brief swap function to allow implementing of copy-and-swap idiom on members of type CEmberGpSinkTableOption
         *
         * This function will swap all attributes of \p first and \p second
         * See http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
         *
         * @param first The first object
         * @param second The second object
         */
        friend void (::swap)(CEmberGpAddressStruct& first, CEmberGpAddressStruct& second); 
        
        /**
         * @brief Assignment operator
         */
        CEmberGpAddressStruct& operator=(CEmberGpAddressStruct other);

        /**
         * @brief The GPD's EUI64.
         */
        EmberEUI64 getGpdIeeeAddress() const { return gpdIeeeAddress; }

        /**
         * @brief The GPD's source ID. not mention in ezsp specification but ieee and sourceId is an union
         */
        uint32_t getSourceId() const { return quad_u8_to_u32(gpdIeeeAddress.at(3), gpdIeeeAddress.at(2), gpdIeeeAddress.at(1), gpdIeeeAddress.at(0)); }

        /**
         * @brief The GPD Application ID.
         */
        uint8_t getApplicationId() const { return applicationId; }

        /**
         * @brief The GPD endpoint.
         */
        uint8_t getEndpoint() const { return endpoint; }

        /**
         * @brief return a raw buffer
         * 
         * @return raw buffer
         */
        std::vector<uint8_t> getRaw(void);

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
        uint8_t applicationId; // The GPD Application ID.
        uint8_t endpoint; // The GPD endpoint.

};
