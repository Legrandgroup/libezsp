/**
 * @file green-power-device.h
 *
 * @brief Represents data for a green power device
 */

#pragma once

#include <cstdint>
#include "../ezsp-protocol/ezsp-enum.h"

/**
 * @brief Class to encapsulate data representing a green power device
 */
class CGpDevice
{
    public:
		static const EmberKeyData UNKNOWN_KEY;

        /**
         * @brief Default constructor
         *
         * Construction without arguments is not allowed
         */
        CGpDevice() = delete;

        /**
         * @brief Constructor with minimal parameter
         * @param i_source_id : source id of gpd
         * @param i_key : key used by the GP device
         */
        CGpDevice(uint32_t i_source_id, const EmberKeyData& i_key);

        /**
         * @brief Assignment operator
         *
         * Copy construction is forbidden on this class
         */
        CGpDevice& operator=(const CGpDevice& other) = delete;

        /**
         * @brief Retrieve the source id for this device
         *
         * @return The source ID
         */
        uint32_t getSourceId() const;

        /**
         * @brief Retrieve the key for this device
         *
         * @return The key
         */
        EmberKeyData getKey() const;

    private:
        uint32_t     source_id;	/*!< The source ID for this device */
        EmberKeyData key; /*!< The key for this device */
};
