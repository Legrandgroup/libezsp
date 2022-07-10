/**
 * @file zcl-frame.h
 *
 * @brief Handles decoding of a zcl frame
 */
#pragma once

#include <spi/ByteBuffer.h>

namespace NSEZSP {

class LIBEXPORT CZclFrame {
public:
	/**
	 * @brief Default constructor
	 *
	 * Construction without arguments is not allowed
	 */
	CZclFrame();

	/**
	 * @brief Assignment operator
	 *
	 * Copy construction is forbidden on this class
	 */
	CZclFrame& operator=(const CZclFrame& other) = default;

	/**
	 * @brief Construction from an incoming ezsp raw message
	 *
	 * @param endpoint Endpoint ID
	 * @param cluster cluster ID
	 * @param type Type
	 * @param command Command ID
	 * @param payload Payload of the command
	 */
	explicit CZclFrame(uint8_t endpoint, uint16_t cluster, uint8_t type, uint8_t command, NSSPI::ByteBuffer payload);

	/**
	 * @brief Dump this instance as a string
	 *
	 * @return The resulting string
	 */
	std::string String() const;

	// getter
	uint8_t getEndpoint() const {
		return endpoint;
	}

	uint16_t getCluster() const {
		return cluster;
	}

	uint8_t getType() const {
		return type;
	}

	uint8_t getCommand() const {
		return command;
	}

	NSSPI::ByteBuffer getPayload() const {
		return payload;
	}

private:
	uint8_t endpoint;
	uint16_t cluster;
	uint8_t type;
	uint8_t command;
	NSSPI::ByteBuffer payload;
};

} // namespace NSEZSP
