/**
 * @file ember-gp-sink-table-entry-struct.h
 *
 * @brief The internal representation of a sink table entry.
 */

#pragma once

#include "../ezsp-enum.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

class CEmberGpSinkTableEntryStruct
{
    public:
        /**
         * @brief Default constructor
         *
         * Construction without arguments is not allowed
         */
        CEmberGpSinkTableEntryStruct() = delete;

        /**
         * @brief Construction from a buffer
         *
         * @param raw_message The buffer to construct from
         */
        CEmberGpSinkTableEntryStruct(const std::vector<uint8_t>& raw_message);

        /**
         * @brief Copy constructor
         *
         * Copy construction is forbidden on this class
         */
        CEmberGpSinkTableEntryStruct(const CEmberGpSinkTableEntryStruct& other) = delete;

        /**
         * @brief Assignment operator
         *
         * Assignment is forbidden on this class
         */
        CEmberGpSinkTableEntryStruct& operator=(const CEmberGpSinkTableEntryStruct& other) = delete;

    private:



};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN