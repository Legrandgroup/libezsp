/**
 * @file ember-child-data-struct.cpp
 *
 * @brief A structure containing a child node's data.
 */

#include <sstream>
#include <iomanip>

#include "ezsp/ezsp-protocol/struct/ember-child-data-struct.h"

#include "ezsp/byte-manip.h"

using NSEZSP::CEmberChildDataStruct;

CEmberChildDataStruct::CEmberChildDataStruct(const std::vector<uint8_t>& raw_message) :
	eui64(),
	type(static_cast<EmberNodeType>(raw_message.at(EMBER_EUI64_BYTE_SIZE))),
	id(static_cast<EmberNodeId>(dble_u8_to_u16(raw_message.at(EMBER_EUI64_BYTE_SIZE+2), raw_message.at(EMBER_EUI64_BYTE_SIZE+1)))),
	phy(raw_message.at(EMBER_EUI64_BYTE_SIZE+3)),
	power(raw_message.at(EMBER_EUI64_BYTE_SIZE+4)),
	timeout(raw_message.at(EMBER_EUI64_BYTE_SIZE+5)),
	gpdIeeeAddress(),	/* FIXME */
	sourceId(),	/* FIXME */
	applicationId(), /* FIXME */
	endpoint() /* FIXME */
{
    for(uint8_t loop=0; loop<EMBER_EUI64_BYTE_SIZE; loop++)
    {
        eui64.push_back(raw_message.at(loop));
    }
    //    if( raw_message.size() > 17 ) // todo associate to node type
    //    {
    //        gpdIeeeAddress.clear();
    //        for(uint8_t loop=0; loop<EMBER_EUI64_BYTE_SIZE; loop++)
    //        {
    //            gpdIeeeAddress.push_back(raw_message.at(EMBER_EUI64_BYTE_SIZE+6+loop));
    //        }
    //
    //        sourceId = (
    //            static_cast<uint32_t>(raw_message.at(EMBER_EUI64_BYTE_SIZE+EMBER_EUI64_BYTE_SIZE+7)) |
    //            static_cast<uint32_t>(raw_message.at(EMBER_EUI64_BYTE_SIZE+EMBER_EUI64_BYTE_SIZE+8))<<8 |
    //            static_cast<uint32_t>(raw_message.at(EMBER_EUI64_BYTE_SIZE+EMBER_EUI64_BYTE_SIZE+9))<<16 |
    //            static_cast<uint32_t>(raw_message.at(EMBER_EUI64_BYTE_SIZE+EMBER_EUI64_BYTE_SIZE+10))<<24
    //        );
    //
    //        applicationId = raw_message.at(EMBER_EUI64_BYTE_SIZE+EMBER_EUI64_BYTE_SIZE+11);
    //
    //        endpoint = raw_message.at(EMBER_EUI64_BYTE_SIZE+EMBER_EUI64_BYTE_SIZE+12);
    //    }
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
