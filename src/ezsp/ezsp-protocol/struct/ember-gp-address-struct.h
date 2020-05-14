/**
 * @file ember-gp-address-struct.h
 *
 * @brief A GP address structure.
 */
#pragma once

#include "ezsp/ezsp-protocol/ezsp-enum.h"
#include "ezsp/byte-manip.h"
#include "spi/ByteBuffer.h"

namespace NSEZSP {

class CEmberGpAddressStruct
{
    public:
        /**
         * @brief Default constructor
         */
        CEmberGpAddressStruct();

        /**
         * @brief Construction from a buffer
         *
         * @param raw_message The buffer to construct from
         */
        explicit CEmberGpAddressStruct(const NSSPI::ByteBuffer& raw_message);

        /**
         * @brief Construct from sourceId
         *
         * @param i_srcId SourceId to construct from
         */
        explicit CEmberGpAddressStruct(const uint32_t i_srcId);

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
        NSSPI::ByteBuffer getRaw() const;

        /**
         * @brief Dump this instance as a string
         *
         * @return The resulting string
         */
		std::string toString() const;

        /**
         * @brief Serialize to an iostream
         *
         * @param out The original output stream
         * @param data The object to serialize
         *
         * @return The new output stream with serialized data appended
         */
        friend std::ostream& operator<< (std::ostream& out, const CEmberGpAddressStruct& data){
			out << data.toString();
			return out;
		}

    private:
        uint8_t applicationId; /*!< The GPD Application ID */
        EmberEUI64 gpdIeeeAddress; /*!< The GPD's EUI64 */
        uint8_t endpoint; /*!< The GPD endpoint */

};

} //namespace NSEZSP
