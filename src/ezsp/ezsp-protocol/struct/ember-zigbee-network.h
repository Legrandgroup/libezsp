/**
 * @file ember-zigbee-network.h
 *
 * @brief A structure containing the parameters of a ZigBee network
 */

#pragma once

//#include "ezsp/ezsp-protocol/ezsp-enum.h"
#include "spi/ByteBuffer.h"

namespace NSEZSP {

class ZigbeeNetworkScanResult;	// Forward declaration

class CEmberZigbeeNetwork {
public:
	/**
	 * @brief Default constructor
	 *
	 * Construction without arguments is not allowed
	 */
	CEmberZigbeeNetwork() = delete;

	/**
	 * @brief Construction from a buffer
	 *
	 * @param raw_message The buffer to construct from
	 */
	explicit CEmberZigbeeNetwork(const NSSPI::ByteBuffer& raw_message);

	/**
	 * @brief Copy constructor
	 *
	 * @param other The object to copy from
	 */
	CEmberZigbeeNetwork(const CEmberZigbeeNetwork& other);

	/**
	 * @brief Assignment operator
	 *
	 * Assignment is forbidden on this class
	 */
	CEmberZigbeeNetwork& operator=(const CEmberZigbeeNetwork& other) = delete;

	// /**
	//  * @brief A bitmask indicating the presence of data within the various fields in the structure.
	//  */
	// EmberKeyStructBitmask getBitmask() const {
	// 	return bitmask;
	// }

	// /**
	//  * @brief The type of the key.
	//  */
	// EmberKeyType getType() const {
	// 	return type;
	// }

	// /**
	//  * @brief The actual key data.
	//  */
	// EmberKeyData getKey() const {
	// 	return key;
	// }

	// /**
	//  * @brief The outgoing frame counter associated with the key.
	//  */
	// uint32_t getOutgoingFrameCounter() const {
	// 	return outgoingFrameCounter;
	// }

	// /**
	//  * The frame counter of the partner device associated
	//  * with the key.
	//  */
	// uint32_t getIncomingFrameCounter() const {
	// 	return incomingFrameCounter;
	// }

	/**
	 * @brief Get the enclosed 802.15.4 channel associated with the network
	 *
	 * @return The enclosed 802.15.4 channel associated with the network
	 */
	uint8_t getChannel() const {
		return this->channel;
	}

	// /**
	//  * @brief The IEEE address of the partner device also in possession of the key.
	//  */
	// EmberEUI64 getPartnerEUI64() const {
	// 	return partnerEUI64;
	// }

	/**
	 * @brief Dump this instance as a string
	 *
	 * @return The resulting string
	 */
	std::string toString() const;

	/**
	 * @brief Serialize to an iostream
	 *
	 * @param out The original output stream
	 * @param data The object to serialize
	 *
	 * @return The new output stream with serialized data appended
	 */
	friend std::ostream& operator<< (std::ostream& out, const CEmberZigbeeNetwork& data) {
		out << data.toString();
		return out;
	}

	friend class NSEZSP::ZigbeeNetworkScanResult;

private:
	uint8_t channel;	/*!< The 802.15.4 channel associated with the network */
	uint16_t panId;	/*!< The network's PAN identifier */
	uint64_t extendedPanId;	/*!< The network's extended PAN identifier */
	bool allowingJoin;	/*!< Whether the network is allowing MAC associations */
	uint8_t stackProfile;	/*!< The Stack Profile associated with the network */
	uint8_t nwkUpdateId;	/*!< The instance of the Network */
};

} // namespace NSEZSP
