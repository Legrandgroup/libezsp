/**
 * 
 */
#include <map>

#include "ezsp-enum.h"

std::string CEzspEnum::EmberNodeTypeToString( EmberNodeType in )
{
    const std::map<EmberNodeType,std::string> MyEnumStrings {
        { EMBER_UNKNOWN_DEVICE, "EMBER_UNKNOWN_DEVICE" },
        { EMBER_COORDINATOR, "EMBER_COORDINATOR" },
        { EMBER_ROUTER, "EMBER_ROUTER" },
        { EMBER_END_DEVICE, "EMBER_END_DEVICE" },
        { EMBER_SLEEPY_END_DEVICE, "EMBER_SLEEPY_END_DEVICE" },
    };
    auto   it  = MyEnumStrings.find(in);
    return it == MyEnumStrings.end() ? "OUT_OF_RANGE" : it->second;   
}

std::string CEzspEnum::EEmberStatusToString( EEmberStatus in )
{
    const std::map<EEmberStatus,std::string> MyEnumStrings {
        { EMBER_SUCCESS, "EMBER_SUCCESS" },
        { EMBER_ERR_FATAL, "EMBER_ERR_FATAL" },
        { EMBER_NO_BUFFERS, "EMBER_NO_BUFFERS" },
        { EMBER_MAC_NO_DATA, "EMBER_MAC_NO_DATA" },
        { EMBER_MAC_INVALID_CHANNEL_MASK, "EMBER_MAC_INVALID_CHANNEL_MASK" },
        { EMBER_MAC_SCANNING, "EMBER_MAC_SCANNING" },
        { EMBER_MAC_NO_ACK_RECEIVED, "EMBER_MAC_NO_ACK_RECEIVED" },
        { EMBER_DELIVERY_FAILED, "EMBER_DELIVERY_FAILED" },
        { EMBER_INVALID_CALL, "EMBER_INVALID_CALL" },
        { EMBER_MAX_MESSAGE_LIMIT_REACHED, "EMBER_MAX_MESSAGE_LIMIT_REACHED" },
        { EMBER_ADDRESS_TABLE_ENTRY_IS_ACTIVE, "EMBER_ADDRESS_TABLE_ENTRY_IS_ACTIVE" },
        { EMBER_NETWORK_UP, "EMBER_NETWORK_UP" },
        { EMBER_NETWORK_DOWN, "EMBER_NETWORK_DOWN" },
        { EMBER_NOT_JOINED, "EMBER_NOT_JOINED" },
        { EMBER_JOIN_FAILED, "EMBER_JOIN_FAILED" },
        { EMBER_MOVE_FAILED, "EMBER_MOVE_FAILED" },
        { EMBER_NETWORK_BUSY, "EMBER_NETWORK_BUSY" },
        { EMBER_NO_BEACONS, "EMBER_NO_BEACONS" },
        { EMBER_RECEIVED_KEY_IN_THE_CLEAR, "EMBER_RECEIVED_KEY_IN_THE_CLEAR" },
        { EMBER_NO_NETWORK_KEY_RECEIVED, "EMBER_NO_NETWORK_KEY_RECEIVED" }
    };
    auto   it  = MyEnumStrings.find(in);
    return it == MyEnumStrings.end() ? "OUT_OF_RANGE" : it->second;      
}

std::string CEzspEnum::EmberJoinMethodToString( EmberJoinMethod in )
{
    const std::map<EmberJoinMethod,std::string> MyEnumStrings {
        { EMBER_USE_MAC_ASSOCIATION, "EMBER_USE_MAC_ASSOCIATION" },
        { EMBER_USE_NWK_REJOIN, "EMBER_USE_NWK_REJOIN" },
        { EMBER_USE_NWK_REJOIN_HAVE_NWK_KEY, "EMBER_USE_NWK_REJOIN_HAVE_NWK_KEY" },
        { EMBER_USE_NWK_COMMISSIONING, "EMBER_USE_NWK_COMMISSIONING" }
    };
    auto   it  = MyEnumStrings.find(in);
    return it == MyEnumStrings.end() ? "OUT_OF_RANGE" : it->second;    
}
