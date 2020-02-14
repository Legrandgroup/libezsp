/**
 * @file ember-gp-address-struct.cpp
 *
 * @brief A GP address structure.
 */
#include <sstream>
#include <iomanip>

#include "ezsp/byte-manip.h"
#include "ember-gp-address-struct.h"

#include "spi/ILogger.h"

using NSEZSP::CEmberGpAddressStruct;

CEmberGpAddressStruct::CEmberGpAddressStruct():
	gpdIeeeAddress({0,0,0,0,0,0,0,0}),
	applicationId(),
	endpoint()
{
}

CEmberGpAddressStruct::CEmberGpAddressStruct(const NSSPI::ByteBuffer& raw_message):
	applicationId(raw_message.at(0)),
	gpdIeeeAddress(),
	endpoint(raw_message.at(EMBER_EUI64_BYTE_SIZE+1))
{
    for (unsigned int loop=0; loop<EMBER_EUI64_BYTE_SIZE; loop++) {
        gpdIeeeAddress.at(loop) = raw_message.at(1+loop);
    }
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

NSSPI::ByteBuffer CEmberGpAddressStruct::getRaw() const
{
    NSSPI::ByteBuffer lo_raw;

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

    buf << "CEmberGpAddressStruct: { ";
    buf << "[applicationId: 0x" << std::hex << std::setw(2) << std::setfill('0') << +static_cast<uint8_t>(applicationId) << "]";
    /* Source ID is duplicated twice in the gpdIeeeAddress, so let's extract the first occurrence to display it */
    buf << "[sourceId: " << std::hex << std::setw(8) << std::setfill('0') << quad_u8_to_u32(gpdIeeeAddress.at(3), gpdIeeeAddress.at(2), gpdIeeeAddress.at(1), gpdIeeeAddress.at(0)) << "]";
    buf << "[gpdIeeeAddress: " << NSSPI::Logger::byteSequenceToString(gpdIeeeAddress) << "]";
    buf << "[endpoint: "<< std::dec << +endpoint << "]";
    buf << " }";

    return buf.str();
}
