/**
 * @file green-power-device.h
 *
 * @brief Represents data for a green power device
 */

#pragma once

#include <cstdint>
#include "ezsp/ezsp-protocol/ezsp-enum.h"
#include "ezsp/ezsp-protocol/struct/ember-gp-sink-table-options-field.h"

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
         */
        CGpDevice& operator=(const CGpDevice other);

        /**
         * @brief swap function to allow implementing of copy-and-swap idiom on members of type CEmberGpSinkTableOption
         *
         * This function will swap all attributes of \p first and \p second
         * See http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
         *
         * @param first The first object
         * @param second The second object
         */
        friend void (::swap)(CGpDevice& first, CGpDevice& second);

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

        /**
         * @brief Retrieve the sink option table for this device
         *
         * @return The option
         */
        CEmberGpSinkTableOption getSinkOption() const { return option; }

        /**
         * @brief Retrieve the sink security option table for this device
         *
         * @return The security option
         */
        uint8_t getSinkSecurityOption() const { return security_option; }

    private:
        uint32_t     source_id;	/*!< The source ID for this device */
        EmberKeyData key; /*!< The key for this device */
        CEmberGpSinkTableOption option; /*!< Sink table option for this device */
        uint8_t security_option; /*!< Sink table security option for this device */
};
