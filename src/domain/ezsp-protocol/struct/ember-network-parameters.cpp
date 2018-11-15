/**
 * 
 */
#include <sstream>
#include <iomanip>

#include "ember-network-parameters.h"


void CEmberNetworkParameters::setRaw(std::vector<uint8_t> raw_message)
{
    extend_pan_id = static_cast<uint64_t>(
        raw_message.at(0) |
        (raw_message.at(1)<<8) |
        (raw_message.at(2)<<16) |
        (raw_message.at(3)<<24) |
        (raw_message.at(4)<<32) |
        (raw_message.at(5)<<40) |
        (raw_message.at(6)<<48) |
        (raw_message.at(7)<<56)
    );

    pan_id = static_cast<uint16_t>(raw_message.at(8) | (raw_message.at(9)<<8));

    radio_tx_power = raw_message.at(10);

    radio_channel = raw_message.at(11);

    join_method = static_cast<EmberJoinMethod>(raw_message.at(12));

    nwk_manager_id = static_cast<EmberNodeId>(raw_message.at(13) | (raw_message.at(14)<<8));

    nwk_update_id = raw_message.at(15);

    channels = static_cast<uint32_t>(
        raw_message.at(16) | 
        (raw_message.at(17)<<8) |
        (raw_message.at(18)<<16) |
        (raw_message.at(19)<<24)
    );

}

std::vector<uint8_t> CEmberNetworkParameters::getRaw()
{
    std::vector<uint8_t> raw_message;

    // extend_pan_id
    raw_message.push_back(static_cast<uint8_t>(extend_pan_id&0xFF));
    raw_message.push_back(static_cast<uint8_t>((extend_pan_id>>8)&0xFF));
    raw_message.push_back(static_cast<uint8_t>((extend_pan_id>>16)&0xFF));
    raw_message.push_back(static_cast<uint8_t>((extend_pan_id>>24)&0xFF));
    raw_message.push_back(static_cast<uint8_t>((extend_pan_id>>32)&0xFF));
    raw_message.push_back(static_cast<uint8_t>((extend_pan_id>>40)&0xFF));
    raw_message.push_back(static_cast<uint8_t>((extend_pan_id>>48)&0xFF));
    raw_message.push_back(static_cast<uint8_t>((extend_pan_id>>56)&0xFF));


    // pan_id
    raw_message.push_back(static_cast<uint8_t>(pan_id&0xFF));
    raw_message.push_back(static_cast<uint8_t>((pan_id>>8)&0xFF));

    // radio_tx_power
    raw_message.push_back(static_cast<uint8_t>(radio_tx_power));

    // radio_channel
    raw_message.push_back(static_cast<uint8_t>(radio_channel));

    // join_method
    raw_message.push_back(static_cast<uint8_t>(join_method));

    // nwk_manager_id
    raw_message.push_back(static_cast<uint8_t>(nwk_manager_id&0xFF));
    raw_message.push_back(static_cast<uint8_t>((nwk_manager_id>>8)&0xFF));

    // nwk_update_id
    raw_message.push_back(static_cast<uint8_t>(nwk_update_id));

    // channels
    raw_message.push_back(static_cast<uint8_t>(channels&0xFF));
    raw_message.push_back(static_cast<uint8_t>((channels>>8)&0xFF));
    raw_message.push_back(static_cast<uint8_t>((channels>>16)&0xFF));
    raw_message.push_back(static_cast<uint8_t>((channels>>24)&0xFF));

    return raw_message;
}

std::string CEmberNetworkParameters::String()
{
    std::stringstream buf;

    buf << "EmberNetworkParameters : { ";
    buf << "[extend_pan_id : "<< std::hex << std::setw(16) << std::setfill('0') << extend_pan_id << "]";
    buf << "[pan_id : "<< std::hex << std::setw(4) << std::setfill('0') << pan_id << "]";
    buf << "[radio_tx_power : "<< std::hex << std::setw(2) << std::setfill('0') << radio_tx_power << "]";
    buf << "[radio_channel : "<< std::hex << std::setw(2) << std::setfill('0') << radio_channel << "]";
    buf << "[join_method : "<< CEzspEnum::EmberJoinMethodToString(join_method) << "]";
    buf << "[nwk_manager_id : "<< std::hex << std::setw(4) << std::setfill('0') << nwk_manager_id << "]";
    buf << "[nwk_update_id : "<< std::hex << std::setw(2) << std::setfill('0') << nwk_update_id << "]";
    buf << "[channels : "<< std::hex << std::setw(8) << std::setfill('0') << channels << "]";
    buf << " }" << std::endl;

    return buf.str();
}
