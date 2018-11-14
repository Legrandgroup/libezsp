/**
 * 
 */

#include <sstream>

#include "get-network-parameters-response.h"

CGetNetworkParamtersResponse::CGetNetworkParamtersResponse(std::vector<uint8_t> raw_message)
{
    status = static_cast<EEmberStatus>(raw_message.at(0));
    node_type = static_cast<EmberNodeType>(raw_message.at(1));
    raw_message.erase(raw_message.begin(),raw_message.begin()+2);
    parameters.setRaw(raw_message);
}

std::string CGetNetworkParamtersResponse::String()
{
    std::stringstream buf;

    buf << "GetNetworkParamtersResponse : { ";
    buf << "[status : "<< CEzspEnum::EEmberStatusToString(status) << "]";
    buf << "[modeType : "<< CEzspEnum::EmberNodeTypeToString(node_type) << "]";
    buf << "["<< parameters.String() << "]";
    buf << " }" << std::endl;

    return buf.str();
}