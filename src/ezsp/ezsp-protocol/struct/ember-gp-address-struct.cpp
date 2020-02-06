/**
 * @file ember-gp-address-struct.cpp
 *
 * @brief A GP address structure.
 */
#include <sstream>
#include <iomanip>

#include "ezsp/byte-manip.h"
#include "ember-gp-address-struct.h"

using NSEZSP::CEmberGpAddressStruct;

CEmberGpAddressStruct::CEmberGpAddressStruct():
	gpdIeeeAddress({0,0,0,0,0,0,0,0}),
	applicationId(),
	endpoint()
{
}

CEmberGpAddressStruct::CEmberGpAddressStruct(const std::vector<uint8_t>& raw_message):
	gpdIeeeAddress(raw_message.begin()+1,raw_message.begin()+1+EMBER_EUI64_BYTE_SIZE),
	applicationId(raw_message.at(0)),
	endpoint(raw_message.at(EMBER_EUI64_BYTE_SIZE+1))
{
}

CEmberGpAddressStruct::CEmberGpAddressStruct(const uint32_t i_srcId):
	// IEEE address will contain the source ID duplicated twice
	gpdIeeeAddress({ u32_get_byte0(i_srcId),
	                 u32_get_byte1(i_srcId),
	                 u32_get_byte2(i_srcId),
	                 u32_get_byte3(i_srcId),
	                 u32_get_byte0(i_srcId),
	                 u32_get_byte1(i_srcId),
	                 u32_get_byte2(i_srcId),
	                 u32_get_byte3(i_srcId) }),
	applicationId(0),
	endpoint(0)
{
}

std::vector<uint8_t> CEmberGpAddressStruct::getRaw() const
{
    std::vector<uint8_t> lo_raw;

    // application Id
    lo_raw.push_back(applicationId);

    // Ieee | sourceId
    for(uint8_t loop=0; loop<EMBER_EUI64_BYTE_SIZE; loop++)
    {
        lo_raw.push_back(gpdIeeeAddress.at(loop));
    }

    // endpoint
    lo_raw.push_back(endpoint);

    return lo_raw;
}

std::string CEmberGpAddressStruct::String() const
{
    std::stringstream buf;

    buf << "CEmberGpAddressStruct : { ";
    buf << "[applicationId : "<< std::hex << std::setw(4) << std::setfill('0') << unsigned(applicationId) << "]";
    buf << "[sourceId : "<< std::hex << std::setw(8) << std::setfill('0') << quad_u8_to_u32(gpdIeeeAddress.at(3), gpdIeeeAddress.at(2), gpdIeeeAddress.at(1), gpdIeeeAddress.at(0)) << "]";
    buf << "[gpdIeeeAddress :";
    for(uint8_t loop=0; loop<gpdIeeeAddress.size(); loop++){ buf << " " << std::hex << std::setw(2) << std::setfill('0') << unsigned(gpdIeeeAddress[loop]); }
    buf << "]";
    buf << "[endpoint : "<< std::dec << std::setw(2) << std::setfill('0') << unsigned(endpoint) << "]";
    buf << " }";

    return buf.str();
}
