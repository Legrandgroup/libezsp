/**
 * @file green-power-sink-table.h
 *
 * @brief A green power sink table
 */
#pragma once

#include <cstdint>
#include <vector>
#include "../zbmessage/green-power-sink-table-entry.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN


#define GP_SINK_INVALID_ENTRY 0xFF

class CGpSinkTable
{
public:
    CGpSinkTable(const CGpSinkTable&) = delete; /* No copy construction allowed */

    CGpSinkTable();

    /**
     * @brief add a green power sink table entry
     *
     * @return index of entry in sink table, or GP_SINK_INVALID_ENTRY if table is full
     */
    uint8_t addEntry( CGpSinkTableEntry i_entry );

    /**
     * @brief remove a green power sink table entry
     *
     * @param i_source_id source id of gpd
     *
     * @return 0 or GP_SINK_INVALID_ENTRY if table entry not found
     */
    uint8_t removeEntry(const uint32_t i_source_id );

    /**
     * @brief obtain entry index according to a gpd source id
     *
     * @param i_source_id source id of gpd
     *
     * @return index of sink table entry, GP_SINK_INVALID_ENTRY if not found
     */
    uint8_t getEntryIndexForSourceId(uint32_t i_source_id);

private:
    std::vector<CGpSinkTableEntry> gpds;

};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
