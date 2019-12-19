/**
 * @file get-network-parameters-response.cpp
 *
 * @brief Handles decoding of the current network parameters.
 */

#include <sstream>

#include "ezsp/ezsp-protocol/get-network-parameters-response.h"

CGetNetworkParamtersResponse::CGetNetworkParamtersResponse(const std::vector<uint8_t>& raw_message) :
	status(static_cast<EEmberStatus>(raw_message.at(0))),
	node_type(static_cast<EmberNodeType>(raw_message.at(1))),
	parameters(raw_message,2)

{
}

std::string CGetNetworkParamtersResponse::String() const
{
    std::stringstream buf;

    buf << "GetNetworkParamtersResponse : { ";
    buf << "[status : "<< CEzspEnum::EEmberStatusToString(status) << "]";
    buf << "[modeType : "<< CEzspEnum::EmberNodeTypeToString(node_type) << "]";
    buf << "["<< parameters.String() << "]";
    buf << " }";

    return buf.str();
}

std::ostream& operator<< (std::ostream& out, const CGetNetworkParamtersResponse& data){
    out << data.String();
    return out;
}
