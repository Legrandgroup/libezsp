/**
 * @file get-network-parameters-response.cpp
 *
 * @brief Handles decoding of the current network parameters.
 */

#include <sstream>

#include "ezsp/ezsp-protocol/get-network-parameters-response.h"

using NSEZSP::CGetNetworkParametersResponse;

CGetNetworkParametersResponse::CGetNetworkParametersResponse(const NSSPI::ByteBuffer& raw_message) :
	status(static_cast<EEmberStatus>(raw_message.at(0))),
	node_type(static_cast<EmberNodeType>(raw_message.at(1))),
	parameters(raw_message,2)

{
}

std::string CGetNetworkParametersResponse::String() const
{
    std::stringstream buf;

    buf << "GetNetworkParametersResponse : { ";
    buf << "[status : "<< CEzspEnum::EEmberStatusToString(status) << "]";
    buf << "[modeType : "<< CEzspEnum::EmberNodeTypeToString(node_type) << "]";
	buf << "["<< parameters.toString() << "]";
    buf << " }";

    return buf.str();
}
