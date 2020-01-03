/**
 * @file green-power-sink-table-entry.cpp
 *
 * @brief Defines one entry in a green power sink table
 */

#include "green-power-sink-table-entry.h"

using namespace NSEZSP;

CGpSinkTableEntry::CGpSinkTableEntry(uint32_t i_source_id) :
    application_id(E_GPD_APPLICATION_SOURCE_ID),
    source_id(i_source_id)
{
}


uint32_t CGpSinkTableEntry::getSourceId()
{
    uint32_t lo_source_id = GP_INVALID_SOURCE_ID;

    if( E_GPD_APPLICATION_SOURCE_ID == application_id )
    {
        lo_source_id = source_id;
    }

    return lo_source_id;
}
