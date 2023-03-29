/**
 * @file device.h
 *
 * @brief device object
 */
#pragma once

#include <spi/ByteBuffer.h>
#include <endpoint.h>
#include <ezsp/ezsp-protocol/ezsp-enum.h>

namespace NSMAIN {

class CDevice {
public:
	/**
	 * @brief Default constructor
	 *
	 * Construction without arguments is not allowed
	 */
	CDevice();

	/**
	 * @brief Assignment operator
	 *
	 * Copy construction is forbidden on this class
	 */
	CDevice& operator=(const CDevice& other) = default;

	/**
	 * @brief Constructor for a full device
	 *
	 * @param endpoint List of endpoint object which contains clusters
	 * @param address address
	 * @param profile_id profile_id ID
	 * @param device_id device ID
	 * @param version version
	 */
	explicit CDevice(NSEZSP::EmberNodeId address, std::vector<CEndpoint> endpoint, uint16_t profile_id, 
                     uint8_t device_id, uint8_t version);

	/**
	 * @brief Constructor with just the node id
	 *
	 * @param address address
	 */
	explicit CDevice(NSEZSP::EmberNodeId address);

	/**
	 * @brief Dump this instance as a string
	 *
	 * @return The resulting string
	 */
	std::string String() const;

	// getter
    NSEZSP::EmberNodeId getAddress() const {
		return address;
	}

	std::vector<CEndpoint> getEndpoint() const {
		return endpoint;
	}

	uint16_t getProfileId() const{
		return profile_id;
	}

	uint8_t getDeviceId() const{
		return device_id;
	}

	uint8_t getVersion() const{
		return version;
	}

	NSEZSP::EmberEUI64 getAddressEUI64() const{
		return this->address_eui64;
	}

	//setter

	void setAddress(NSEZSP::EmberNodeId address){
		this->address = address;
	}

	void setProfileId(uint16_t profile_id){
		this->profile_id = profile_id;
	}

	void setDeviceId(uint16_t device_id){
		this->device_id = device_id;
	}

	void setVersion(uint8_t version){
		this->version = version;
	}

	void setEndpoint(CEndpoint endpoint){
		this->endpoint.push_back(endpoint);
	}

	void setAddressEUI64(NSEZSP::EmberEUI64 addressEUI64){
		this->address_eui64 = addressEUI64;
	}

private:
	std::vector<CEndpoint> endpoint; // List of active endpoints on device
	NSEZSP::EmberEUI64 address_eui64; // EUI64 of device
	NSEZSP::EmberNodeId address; // NodeID
	uint16_t profile_id;
	uint16_t device_id;
	uint8_t version;
};

} // namespace NSEZSP
