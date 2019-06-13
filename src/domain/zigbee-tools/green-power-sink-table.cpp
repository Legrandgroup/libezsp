/**
 * @file green-power-sink-table.cpp
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
            lo_index = gpds.size();
            gpds.push_back(i_entry);
        }
    }

//    clogI << "CGpSinkTable::addEntry at index : " << unsigned(lo_index) << std::endl;

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

    for( int loop=0; loop<gpds.size(); loop++ )
    {
        if( i_source_id == gpds.at(loop).getSourceId() )
        {
            lo_index = loop;
            loop = gpds.size();
        }
    }

//    clogI << "CGpSinkTable::getEntryIndexForSourceId at index : " << unsigned(lo_index) << std::endl;

    return lo_index;
}