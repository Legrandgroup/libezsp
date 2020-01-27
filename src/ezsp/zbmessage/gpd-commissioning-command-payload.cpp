/**
 * @file gpd-commissioning-command-payload.cpp
 *
 * @brief Decoding payload of gpd commissioning command according to A.4.2.1.1 GPD Commissioning command from docs-14-0563-16-batt-green-power-spec_ProxyBasic.pdf
 */

#include <sstream>
#include <iomanip>
#include <cstring>

#include "ezsp/byte-manip.h"
#include "spi/aes/AesBuilder.h"
#include "ezsp/zbmessage/gpd-commissioning-command-payload.h"

using NSEZSP::CGpdCommissioningPayload;

CGpdCommissioningPayload::CGpdCommissioningPayload(const std::vector<uint8_t>& raw_message, uint32_t i_src_id):
        device_id(raw_message.at(0)),
        options(raw_message.at(1)),
        extended_options(0),
        key(),
        key_mic(),
        out_frame_counter(),
        app_information(0),
        manufacturer_id(),
        model_id(),
        gpd_command_list(),
        gpd_cluster_list()
{
    // only device_id and option are mandatory, other field depend of option value
    unsigned int l_idx = 2;

    // extended option
    if( options & (1<<COM_OPTION_EXTENDED_OPTION_FIELD_BIT) )
    {
        extended_options = raw_message.at(l_idx);
        l_idx++;
    }

    // gpd key
    if( extended_options & (1<<COM_EXT_OPTION_GPD_KEY_PRESENT_BIT) )
    {
        key.insert(key.begin(),raw_message.begin()+static_cast<int>(l_idx),raw_message.begin()+static_cast<int>(l_idx)+EMBER_KEY_DATA_BYTE_SIZE);
        l_idx += EMBER_KEY_DATA_BYTE_SIZE;
        // gpd key MIC and encryption
        if( extended_options & (1<<COM_EXT_OPTION_GPD_KEY_ENCRYPTION_BIT) )
        {
            // MIC
            key_mic = quad_u8_to_u32(raw_message.at(l_idx+3),raw_message.at(l_idx+2),raw_message.at(l_idx+1),raw_message.at(l_idx));
            l_idx += 4;
            // uncrypt key using default TC-LK (A.3.3.3.3 gpLinkKey:‘ZigBeeAlliance09’) with method A.3.7.1.2.3 Over- the-air protection of GPD key with TC-LK
            uint8_t TC_LK[16] = {0x5A, 0x69, 0x67, 0x42, 0x65, 0x65, 0x41, 0x6C, 0x6C, 0x69, 0x61, 0x6E, 0x63, 0x65, 0x30, 0x39};
            uint8_t nonce[16];
            uint8_t in_key[16];
            uint8_t out_key[16];
            NSSPI::IAes *aes = NSSPI::AesBuilder::create();
            // fill in_key
            memcpy(in_key,key.data(),16);
            // fill out key
            memset(out_key,0,16);
            //construct nonce
            memset(nonce,0,16);
            nonce[0] = 0x01;
            memcpy(&nonce[1], static_cast<const void *>(&i_src_id), 4);
            memcpy(&nonce[5], static_cast<const void *>(&i_src_id), 4);
            memcpy(&nonce[9], static_cast<const void *>(&i_src_id), 4);
            nonce[13] = 0x05;
            nonce[14]=0x00;
            nonce[15]=0x01;

            // decrypt
            aes->set_key(TC_LK);
            aes->xor_block(out_key, nonce);
            aes->encrypt(out_key, out_key);
            aes->xor_block(out_key, in_key);

            // fill key with uncrypt value
            key.clear();
            for(int loop=0; loop<16; loop++){ key.push_back(out_key[loop]);}

            // verify MIC
            // \todo
        }
    }

    // gpd outgoing counter
    if( extended_options & (1<<COM_EXT_OPTION_GPD_OUT_COUNTER_PRESENT_BIT) )
    {
        out_frame_counter = quad_u8_to_u32(raw_message.at(l_idx+3),raw_message.at(l_idx+2),raw_message.at(l_idx+1),raw_message.at(l_idx));
            l_idx += 4;
    }

    // application information
    if( options & (1<<COM_OPTION_APPLICATION_INFORMATION_BIT) )
    {
        app_information = raw_message.at(l_idx);
        l_idx++;
    }

    // manufacturer id
    if( app_information & (1<<COM_APP_INFO_MANUFACTURER_ID_PRESENT_BIT) )
    {
        manufacturer_id = dble_u8_to_u16(raw_message.at(l_idx+1),raw_message.at(l_idx));
        l_idx += 2;
    }

    // model id
    if( app_information & (1<<COM_APP_INFO_MODEL_ID_PRESENT_BIT) )
    {
        model_id = dble_u8_to_u16(raw_message.at(l_idx+1),raw_message.at(l_idx));
        l_idx += 2;
    }

    // gpd commands
    if( app_information & (1<<COM_APP_INFO_GPD_COMMANDS_PRESENT_BIT) )
    {
        uint8_t l_number = raw_message.at(l_idx);
        l_idx++;
        for( int loop=0; loop<l_number; loop++)
        {
            gpd_command_list.push_back(raw_message.at(l_idx));
            l_idx++;
        }
    }

    // gpd cluster list
    if( app_information & (1<<COM_APP_INFO_CLUSTER_LIST_PRESENT_BIT) )
    {
        gpd_cluster_list.insert(gpd_cluster_list.begin(),raw_message.begin()+static_cast<int>(l_idx),raw_message.end());
    }
}

std::string CGpdCommissioningPayload::String() const
{
    std::stringstream buf;

    buf << "CGpdCommissioningPayload : { ";
    buf << "[device_id : "<< std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(device_id) << "]";
    buf << "[options : "<< std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(options) << "]";
    buf << "[extended_options : "<< std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(extended_options) << "]";
    buf << "[key : ";
    for(uint8_t loop=0; loop<key.size(); loop++){ buf << " " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(key[loop]); }
    buf << "]";
    buf << "[key_mic : "<< std::hex << std::setw(8) << std::setfill('0') << static_cast<unsigned int>(key_mic) << "]";
    buf << "[out_frame_counter : "<< std::hex << std::setw(8) << std::setfill('0') << static_cast<unsigned int>(out_frame_counter) << "]";
    buf << "[app_information : "<< std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(app_information) << "]";
    buf << "[manufacturer_id : "<< std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(manufacturer_id) << "]";
    buf << "[model_id : "<< std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(model_id) << "]";
    buf << "[gpd_command_list :";
    for(uint8_t loop=0; loop<gpd_command_list.size(); loop++){ buf << " " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(gpd_command_list[loop]); }
    buf << "]";
    buf << "[gpd_cluster_list :";
    for(uint8_t loop=0; loop<gpd_cluster_list.size(); loop++){ buf << " " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(gpd_cluster_list[loop]); }
    buf << "]";
    buf << " }";

    return buf.str();
}
