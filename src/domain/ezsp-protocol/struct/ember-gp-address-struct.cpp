/**
 * @file ember-gp-address-struct.cpp
 *
 * @brief A GP address structure.
 */
#include <sstream>
#include <iomanip>
#include "../../byte-manip.h"

#include "ember-gp-address-struct.h"

CEmberGpAddressStruct::CEmberGpAddressStruct(const std::vector<uint8_t>& raw_message):
	gpdIeeeAddress(),	/* FIXME */
	sourceId(0),
	applicationId(0),
	endpoint(0)
{
    for(uint8_t loop=0; loop<EMBER_EUI64_BYTE_SIZE; loop++)
    {
        gpdIeeeAddress.push_back(raw_message.at(loop));
    }

    // not mention in ezsp specification but ieee and sourceId is an union
    sourceId = quad_u8_to_u32(gpdIeeeAddress.at(2), gpdIeeeAddress.at(3), gpdIeeeAddress.at(0), gpdIeeeAddress.at(1));

    // if application id equal 0, so sourceId must be used, else ieee and endpoint
    applicationId = raw_message.at(EMBER_EUI64_BYTE_SIZE);

    // endpoint only if application Id is not 0
    endpoint = raw_message.at(EMBER_EUI64_BYTE_SIZE+1);
}


std::string CEmberGpAddressStruct::String() const
{
    std::stringstream buf;

    buf << "CEmberGpAddressStruct : { ";
    buf << "[gpdIeeeAddress :";
    for(uint8_t loop=0; loop<gpdIeeeAddress.size(); loop++){ buf << " " << std::hex << std::setw(2) << std::setfill('0') << unsigned(gpdIeeeAddress[loop]); }
    buf << "]";
    buf << "[sourceId : "<< std::hex << std::setw(8) << std::setfill('0') << sourceId << "]";
    buf << "[applicationId : "<< std::hex << std::setw(4) << std::setfill('0') << unsigned(applicationId) << "]";
    buf << "[endpoint : "<< std::dec << std::setw(2) << std::setfill('0') << unsigned(endpoint) << "]";
    buf << " }";

    return buf.str();
}

std::ostream& operator<< (std::ostream& out, const CEmberGpAddressStruct& data){
    out << data.String();
    return out;
}