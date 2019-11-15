/**
 * @file green-power-sink-table.cpp
 *
 * @brief A green power sink table
 */

#include <sstream>
#include <iomanip>

#include "green-power-sink-table.h"

#include "../../spi/GenericLogger.h"
#include "../../spi/ILogger.h"

CGpSinkTable::CGpSinkTable() :
    gpds()
{

}

uint8_t CGpSinkTable::addEntry( CGpSinkTableEntry i_entry )
{
    uint8_t lo_index = GP_SINK_INVALID_ENTRY;

/*
    std::stringstream buf;
    buf << "[source_id : "<< std::hex << std::setw(8) << std::setfill('0') << unsigned(i_entry.getSourceId()) << "]";
    clogI << "CGpSinkTable::addEntry : " << buf.str() << std::endl;
*/
    // loop on table to search if gpd is already register
    lo_index = getEntryIndexForSourceId( i_entry.getSourceId() );
    if( GP_SINK_INVALID_ENTRY == lo_index )
    {
        // limit number of entry !
        if( gpds.size() < GP_SINK_INVALID_ENTRY )
        {
            if (gpds.size()>255)
            {
                clogW << "gpd table size overflow\n";
            }
            else
            {
                lo_index = static_cast<uint8_t>(gpds.size());
                gpds.push_back(i_entry);
            }
        }
    }

//    clogI << "CGpSinkTable::addEntry at index : " << unsigned(lo_index) << std::endl;

    return lo_index;
}

uint8_t CGpSinkTable::removeEntry(const uint32_t i_source_id )
{
    uint8_t lo_index = GP_SINK_INVALID_ENTRY;

/*
    std::stringstream buf;
    buf << "[source_id : "<< std::hex << std::setw(8) << std::setfill('0') << unsigned(i_source_id) << "]";
    clogI << "CGpSinkTable::removeEntry : " << buf.str() << std::endl;
*/
    // loop on table to search if gpd is already register
    lo_index = getEntryIndexForSourceId( i_source_id );
//    clogI << "CGpSinkTable::removeEntry at index : " << unsigned(lo_index) << std::endl;
    if( GP_SINK_INVALID_ENTRY != lo_index )
    {
        const auto it = gpds.begin() + lo_index;
        gpds.erase(it);
        return 0;
    }

    return lo_index;
}

uint8_t CGpSinkTable::getEntryIndexForSourceId(uint32_t i_source_id)
{
    uint8_t lo_index = GP_SINK_INVALID_ENTRY;

/*
    std::stringstream buf;
    buf << "[source_id : "<< std::hex << std::setw(8) << std::setfill('0') << unsigned(i_source_id) << "]";
    clogI << "CGpSinkTable::getEntryIndexForSourceId : " << buf.str() << std::endl;
*/

    for( unsigned int loop=0; loop<gpds.size(); loop++ )
    {
        if( i_source_id == gpds.at(loop).getSourceId() )
        {
            if (loop>255)
            {
                clogW << "gpd table index overflow\n";
            }
            else
            {
                lo_index = static_cast<uint8_t>(loop);
                break;
            }
        }
    }

//    clogI << "CGpSinkTable::getEntryIndexForSourceId at index : " << unsigned(lo_index) << std::endl;

    return lo_index;
}
