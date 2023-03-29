/**
 * @file device.h
 *
 * @brief device object
 */

#include <sstream>
#include <iomanip>
#include <iostream>

#include "device.h"

using NSMAIN::CDevice;

CDevice::CDevice():
    endpoint(0),
    address(0),
    profile_id(0),
    device_id(0),
    version(0){
}

CDevice::CDevice(NSEZSP::EmberNodeId address, std::vector<CEndpoint> endpoint, uint16_t profile_id, 
                 uint8_t device_id, uint8_t version):
    endpoint(0),
    address(0),
    profile_id(0),
    device_id(0),
    version(0){
    
    this->endpoint = endpoint;
    this->address = address;
    this->profile_id = profile_id;
    this->device_id = device_id;
    this->version = version;
}

CDevice::CDevice(NSEZSP::EmberNodeId address):
    endpoint(0),
    address(0),
    profile_id(0),
    device_id(0),
    version(0){
        this->address = address;
}

/**
 * @brief Dump this instance as a string
 *
 * @return The resulting string
 */
std::string CDevice::String() const{
    std::stringstream buf;

	buf << "Device: { ";
    buf << "[address: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(this->address) << "]";
    buf << "[eui64: ";
    for(uint8_t loop; loop<this->address_eui64.size(); loop++){
        if(loop == this->address_eui64.size()-1) buf << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(this->address_eui64[loop]);
        else buf << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(this->address_eui64[loop]) << ":";
    }
    buf << "]";
	buf << "[profile_id: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(this->profile_id) << "]";
	buf << "[device_id: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(this->device_id) << "]";
	buf << "[version: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(this->version) << "]";
    for(uint8_t loop = 0; loop < this->endpoint.size(); loop++) {
        buf << "[ endpoint : ";
        buf << std::hex << std::setw(2) << std::setfill('0') << unsigned(this->endpoint[loop].getEndpoint());
        buf << " { in : ";
        for(uint8_t i = 0; i < this->endpoint[loop].getClusterIn().size(); i++) {
            buf << std::hex << std::setw(4) << std::setfill('0') << unsigned(this->endpoint[loop].getClusterIn()[i]) << " ";
        }
        buf << "}";
        buf << "{ out : ";
        for(uint8_t i = 0; i < this->endpoint[loop].getClusterOut().size(); i++) {
            buf << std::hex << std::setw(4) << std::setfill('0') << unsigned(this->endpoint[loop].getClusterOut()[i]) << " ";
        }
        buf << "}]";
    }
    
	buf << " }";

	return buf.str();
}