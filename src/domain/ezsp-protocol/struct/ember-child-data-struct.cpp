/**
 * 
 */
#include <sstream>
#include <iomanip>

#include "ember-child-data-struct.h"


void CEmberChildDataStruct::setRaw(std::vector<uint8_t> raw_message)
{
    eui64.clear();
    for(uint8_t loop=0; loop<EMBER_EUI64_BYTE_SIZE; loop++)
    {
        eui64.push_back(raw_message.at(loop));
    }

    type = static_cast<EmberNodeType>(raw_message.at(EMBER_EUI64_BYTE_SIZE));

    id = static_cast<EmberNodeId>(raw_message.at(EMBER_EUI64_BYTE_SIZE+1) | (raw_message.at(EMBER_EUI64_BYTE_SIZE+2)<<8));

    phy = static_cast<uint8_t>(raw_message.at(EMBER_EUI64_BYTE_SIZE+3));

    power = static_cast<uint8_t>(raw_message.at(EMBER_EUI64_BYTE_SIZE+4));

    timeout = static_cast<uint8_t>(raw_message.at(EMBER_EUI64_BYTE_SIZE+5));
/*
    if( raw_message.size() > 17 ) // todo associate to node type
    {
        gpdIeeeAddress.clear();
        for(uint8_t loop=0; loop<EMBER_EUI64_BYTE_SIZE; loop++)
        {
            gpdIeeeAddress.push_back(raw_message.at(EMBER_EUI64_BYTE_SIZE+6+loop));
        }

        sourceId = static_cast<uint32_t>(
            raw_message.at(EMBER_EUI64_BYTE_SIZE+EMBER_EUI64_BYTE_SIZE+7) | 
            (raw_message.at(EMBER_EUI64_BYTE_SIZE+EMBER_EUI64_BYTE_SIZE+8)<<8) |
            (raw_message.at(EMBER_EUI64_BYTE_SIZE+EMBER_EUI64_BYTE_SIZE+9)<<16) |
            (raw_message.at(EMBER_EUI64_BYTE_SIZE+EMBER_EUI64_BYTE_SIZE+10)<<24)
        );     

        applicationId = static_cast<uint8_t>(raw_message.at(EMBER_EUI64_BYTE_SIZE+EMBER_EUI64_BYTE_SIZE+11));
        
        endpoint = static_cast<uint8_t>(raw_message.at(EMBER_EUI64_BYTE_SIZE+EMBER_EUI64_BYTE_SIZE+12));
    }
*/    
}

std::string CEmberChildDataStruct::String() const
{
    std::stringstream buf;

    buf << "EmberChildDataStruct : { ";
    buf << "[eui64 :";
    for(uint8_t loop=0; loop<eui64.size(); loop++){ buf << " " << std::hex << std::setw(2) << std::setfill('0') << unsigned(eui64[loop]); }
    buf << "]";
    buf << "[type : "<< CEzspEnum::EmberNodeTypeToString(type) << "]";
    buf << "[id : "<< std::hex << std::setw(4) << std::setfill('0') << unsigned(id) << "]";
    buf << "[phy : "<< std::dec << std::setw(2) << std::setfill('0') << unsigned(phy) << "]";
    buf << "[power : "<< std::dec << std::setw(2) << std::setfill('0') << unsigned(power) << "]";
    buf << "[timeout : "<< std::dec << std::setw(2) << std::setfill('0') << unsigned(timeout) << "]";
/* // todo associate to node type    
    buf << "[gpdIeeeAddress :";
    for(uint8_t loop=0; loop<gpdIeeeAddress.size(); loop++){ buf << " " << std::hex << std::setw(2) << std::setfill('0') << unsigned(gpdIeeeAddress[loop]); }
    buf << "]";
    buf << "[sourceId : "<< std::hex << std::setw(8) << std::setfill('0') << sourceId << "]";
    buf << "[applicationId : "<< std::hex << std::setw(4) << std::setfill('0') << unsigned(applicationId) << "]";
    buf << "[endpoint : "<< std::dec << std::setw(2) << std::setfill('0') << unsigned(endpoint) << "]";
*/    
    buf << " }";

    return buf.str();
}
