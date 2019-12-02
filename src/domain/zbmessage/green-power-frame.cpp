/**
 * @file green-power-frame.cpp
 *
 * @brief Handles decoding of a green power frame @todo encoding
 */

#include <sstream>
#include <iomanip>

#include "../byte-manip.h"
#include "green-power-frame.h"

#include "../ezsp-protocol/struct/ember-gp-address-struct.h"

CGpFrame::CGpFrame():
    link_value(0),
    sequence_number(0),
    source_id(0),
    security(GPD_NO_SECURITY),
    key_type(GPD_KEY_TYPE_NO_KEY),
    auto_commissioning(false),
    rx_after_tx(false),
    security_frame_counter(0),
    command_id(0xFF),
    mic(0),
    proxy_table_entry(0xFF),
    payload()
{
}

CGpFrame::CGpFrame(const std::vector<uint8_t>& raw_message):
    link_value(0),
    sequence_number(0),
    source_id(0),
    security(GPD_NO_SECURITY),
    key_type(GPD_KEY_TYPE_NO_KEY),
    auto_commissioning(false),
    rx_after_tx(false),
    security_frame_counter(0),
    command_id(0xFF),
    mic(0),
    proxy_table_entry(0xFF),
    payload()
{
    std::vector<uint8_t> l_gp_addr(raw_message.begin()+3,raw_message.end());

    CEmberGpAddressStruct gp_address = CEmberGpAddressStruct(l_gp_addr);
    /* only sourceId addressing mode is supported */
    if( 0 == gp_address.getApplicationId() )
    {
        link_value = raw_message.at(1);
        sequence_number = raw_message.at(2);
        source_id = gp_address.getSourceId();
        security = static_cast<EGpSecurityLevel>(raw_message.at(13));
        key_type = static_cast<EGpSecurityKeyType>(raw_message.at(14));
        auto_commissioning = raw_message.at(15);
        rx_after_tx = raw_message.at(16);
        security_frame_counter = quad_u8_to_u32(raw_message.at(20), raw_message.at(19), raw_message.at(18), raw_message.at(17));
        command_id = raw_message.at(21);
        mic = quad_u8_to_u32(raw_message.at(25), raw_message.at(24), raw_message.at(23), raw_message.at(22));
        proxy_table_entry = raw_message.at(26);
        for( unsigned int loop=0; loop<raw_message.at(27); loop++ )
        {
            payload.push_back(raw_message.at(28+loop));
        }
    }
}

std::string CGpFrame::String() const
{
    std::stringstream buf;

    buf << "CGpFrame : { ";
    buf << "[link_value : "<< std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(link_value) << "]";
    buf << "[sequence_number : "<< std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(sequence_number) << "]";
    buf << "[source_id : "<< std::hex << std::setw(8) << std::setfill('0') << static_cast<unsigned int>(source_id) << "]";
    buf << "[security : "<< std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(security) << "]";
    buf << "[key_type : "<< std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(key_type) << "]";
    buf << "[auto_commissioning : "<< std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(auto_commissioning) << "]";
    buf << "[rx_after_tx : "<< std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(rx_after_tx) << "]";
    buf << "[security_frame_counter : "<< std::hex << std::setw(8) << std::setfill('0') << static_cast<unsigned int>(security_frame_counter) << "]";
    buf << "[command_id : "<< std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(command_id) << "]";
    buf << "[mic : "<< std::hex << std::setw(8) << std::setfill('0') << static_cast<unsigned int>(mic) << "]";
    buf << "[proxy_table_entry : "<< std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(proxy_table_entry) << "]";
    buf << "[payload :";
    for(uint8_t loop=0; loop<payload.size(); loop++){ buf << " " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(payload[loop]); }
    buf << "]";
    buf << " }";

    return buf.str();
}

std::ostream& operator<< (std::ostream& out, const CGpFrame& data){
    out << data.String();
    return out;
}
