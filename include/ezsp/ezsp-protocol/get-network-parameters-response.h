/**
 * @file get-network-parameters-response.h
 *
 * @brief Handles decoding of the current network parameters.
 */

#pragma once

#include "ezsp/ezsp-protocol/ezsp-enum.h"
#include "ezsp/ezsp-protocol/struct/ember-network-parameters.h"
#include "spi/ByteBuffer.h"

namespace NSEZSP {

class CGetNetworkParametersResponse {
public:
	// /**
	//  * @brief Default constructor
	//  *
	//  * Construction without arguments is not allowed
	//  */
	// CGetNetworkParametersResponse() = delete;

	/**
	 * @brief Construction from a buffer
	 *
	 * @param raw_message The buffer to construct from
	 */
	explicit CGetNetworkParametersResponse(const NSSPI::ByteBuffer& raw_message);

	// /**
	//  * @brief Copy constructor
	//  *
	//  * Copy construction is forbidden on this class
	//  */
	// CGetNetworkParametersResponse(const CGetNetworkParametersResponse& other) = delete;

	// /**
	//  * @brief Assignment operator
	//  *
	//  * Assignment is forbidden on this class
	//  */
	// CGetNetworkParametersResponse& operator=(const CGetNetworkParametersResponse& other) = delete;

	/**
	 * @brief An EmberStatus value indicating success or the reason for failure.
	 */
	EEmberStatus getStatus() const {
		return status;
	}

	/**
	 * @brief An EmberNodeType value indicating the current node type.
	 */
	EmberNodeType getNodeType() const {
		return node_type;
	}

	/**
	 * @brief The current network parameters.
	 */
	CEmberNetworkParameters getParameters() const {
		return parameters;
	}

	/**
	 * @brief Dump this instance as a string
	 *
	 * @return The resulting string
	 */
	std::string String() const;

	/**
	 * @brief Serialize to an iostream
	 *
	 * @param out The original output stream
	 * @param data The object to serialize
	 *
	 * @return The new output stream with serialized data appended
	 */
	friend std::ostream& operator<< (std::ostream& out, const CGetNetworkParametersResponse& data) {
		out << data.String();
		return out;
	}


private:
	EEmberStatus status;
	EmberNodeType node_type;
	CEmberNetworkParameters parameters;
};

} // namespace NSEZSP
