/**
 * @file ember-gp-address-struct.cpp
 *
 * @brief A GP address structure.
 */
#include <sstream>
#include <iomanip>

#include "ember-gp-address-struct.h"

CEmberGpAddressStruct::CEmberGpAddressStruct():
	gpdIeeeAddress({0,0,0,0,0,0,0,0}),
	applicationId(),
	endpoint()
{
}

CEmberGpAddressStruct::CEmberGpAddressStruct(const CEmberGpAddressStruct& other):
	gpdIeeeAddress(other.getGpdIeeeAddress()),
	applicationId(other.getApplicationId()),
	endpoint(other.getEndpoint())
{
}

CEmberGpAddressStruct::CEmberGpAddressStruct(const std::vector<uint8_t>& raw_message):
	gpdIeeeAddress(raw_message.begin()+1,raw_message.begin()+1+EMBER_EUI64_BYTE_SIZE),
	applicationId(raw_message.at(0)),
	endpoint(raw_message.at(EMBER_EUI64_BYTE_SIZE+1))
{
}

/**
 * This method is a friend of CEmberGpAddressStruct class
 * swap() is needed within operator=() to implement to copy and swap paradigm
**/
void swap(CEmberGpAddressStruct& first, CEmberGpAddressStruct& second) /* nothrow */
{
  using std::swap;	// Enable ADL

  swap(first.gpdIeeeAddress, second.gpdIeeeAddress);
  swap(first.applicationId, second.applicationId);
  swap(first.endpoint, second.endpoint);
  /* Once we have swapped the members of the two instances... the two instances have actually been swapped */
}

CEmberGpAddressStruct& CEmberGpAddressStruct::operator=(CEmberGpAddressStruct other)
{
  swap(*this, other);
  return *this;
}


CEmberGpAddressStruct::CEmberGpAddressStruct(const uint32_t i_srcId):
	gpdIeeeAddress(),	/* FIXME */
	applicationId(0),
	endpoint(0)
{
    // update Ieee with twice SourceId
    gpdIeeeAddress.push_back(static_cast<uint8_t>(i_srcId&0xFF));
    gpdIeeeAddress.push_back(static_cast<uint8_t>((i_srcId>>8)&0xFF));
    gpdIeeeAddress.push_back(static_cast<uint8_t>((i_srcId>>16)&0xFF));
    gpdIeeeAddress.push_back(static_cast<uint8_t>((i_srcId>>24)&0xFF));
    gpdIeeeAddress.push_back(static_cast<uint8_t>(i_srcId&0xFF));
    gpdIeeeAddress.push_back(static_cast<uint8_t>((i_srcId>>8)&0xFF));
    gpdIeeeAddress.push_back(static_cast<uint8_t>((i_srcId>>16)&0xFF));
    gpdIeeeAddress.push_back(static_cast<uint8_t>((i_srcId>>24)&0xFF));
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

std::ostream& operator<< (std::ostream& out, const CEmberGpAddressStruct& data){
    out << data.String();
    return out;
}
