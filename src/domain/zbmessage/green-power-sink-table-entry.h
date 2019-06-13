/**
 * @file green-power-sink-table-entry.h
 *
 * @brief Handles decoding of a green power frame
 */
#pragma once

#include <cstdint>

#define GP_INVALID_SOURCE_ID 0xFFFFFFFF

typedef enum {
    E_GPD_APPLICATION_SOURCE_ID = 0x0,
    E_GPD_APPLICATION_IEEE      = 0x2
}EGpdApplicationId;

class CGpSinkTableEntry
{
    public:
        /**
         * @brief Default constructor
         *
         * Construction without arguments is not allowed
         */
        CGpSinkTableEntry() = delete;

        /**
         * @brief Assignment operator
         *
         * Copy construction is forbidden on this class
         */
        CGpSinkTableEntry& operator=(const CGpSinkTableEntry& other) = delete;

        /**
         * @brief Constructor with minimal parameter
         * @param[IN] i_source_id : source id of gpd, assume application id is E_GPD_APPLICATION_SOURCE_ID
         * @param[IN] i_key : associate key
         */
        CGpSinkTableEntry(uint32_t i_source_id);

        /**
         * @brief retrieve source id of an entry
         * @return : if entry is type of sourceId, return it, otherwize return GP_INVALID_SOURCE_ID
         */
        uint32_t getSourceId();

private:
    EGpdApplicationId application_id;
    uint32_t source_id;


};