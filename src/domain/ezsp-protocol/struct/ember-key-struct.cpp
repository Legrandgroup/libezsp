/**
 * 
 */
#include <sstream>
#include <iomanip>

#include "ember-key-struct.h"

#include "../../byte-manip.h"

void CEmberKeyStruct::setRaw(std::vector<uint8_t> raw_message)
{
    bitmask = static_cast<EmberKeyStructBitmask>(dble_u8_to_u16(raw_message.at(1), raw_message.at(0)));
    type = static_cast<EmberKeyType>(raw_message.at(2));
    key.clear();
    for(uint8_t loop=0; loop<EMBER_KEY_DATA_BYTE_SIZE; loop++)
    {
        key.push_back( raw_message.at(3U+loop) );
    }
    outgoingFrameCounter = (
        static_cast<uint32_t>(raw_message.at(19)) |
        static_cast<uint32_t>(raw_message.at(20))<<8 |
        static_cast<uint32_t>(raw_message.at(21))<<16 |
        static_cast<uint32_t>(raw_message.at(22))<<24
    );        
    incomingFrameCounter = (
        static_cast<uint32_t>(raw_message.at(23)) |
        static_cast<uint32_t>(raw_message.at(24))<<8 |
        static_cast<uint32_t>(raw_message.at(25))<<16 |
        static_cast<uint32_t>(raw_message.at(26))<<24
    );   
    sequenceNumber = static_cast<EmberKeyType>(raw_message.at(27));
    partnerEUI64.clear();
    for(uint8_t loop=0; loop<EMBER_EUI64_BYTE_SIZE; loop++)
    {
        partnerEUI64.push_back(raw_message.at(28U+loop));
    }
}


std::string CEmberKeyStruct::String()
{
    std::stringstream buf;

    buf << "EmberKeyStruct : { ";
    buf << "[bitmask : "<< std::hex << std::setw(4) << std::setfill('0') << bitmask << "]";
    buf << "[type : "<< CEzspEnum::EmberKeyTypeToString(type) << "]";
    buf << "[key :";
    for(uint8_t loop=0; loop<key.size(); loop++){ buf << " " << std::hex << std::setw(2) << std::setfill('0') << unsigned(key.at(loop)); }
    buf << "]";
    buf << "[outgoingFrameCounter : "<< std::hex << std::setw(8) << std::setfill('0') << outgoingFrameCounter << "]";
    buf << "[incomingFrameCounter : "<< std::hex << std::setw(8) << std::setfill('0') << incomingFrameCounter << "]";
    buf << "[sequenceNumber : "<< std::hex << std::setw(2) << std::setfill('0') << sequenceNumber << "]";
    buf << "[partnerEUI64 :";
    for(uint8_t loop=0; loop<partnerEUI64.size(); loop++){ buf << " " << std::hex << std::setw(2) << std::setfill('0') << unsigned(partnerEUI64[loop]); }
    buf << "]";
    buf << " }";

    return buf.str();
}
