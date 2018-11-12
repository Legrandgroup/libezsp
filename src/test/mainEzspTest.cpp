

#include <iostream>
#include <sstream>
#include <iomanip>

#include "mainEzspTest.h"


CAppDemo::CAppDemo(IUartDriver *uartDriver) : dongle(this) { 
    // uart
    if( dongle.open(uartDriver) )
    {
        // first request stack protocol version
        std::vector<uint8_t> payload;
        payload.push_back(6U);
        dongle.sendCommand(EZSP_VERSION,payload,[&](EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive){
            
            if( EZSP_VERSION == i_cmd )
            {
                // check if the wanted protocol version, and display stack version
                if( 6 == i_msg_receive.at(0) )
                {
                    // all is good
                    std::cout << "Stack version : " << i_msg_receive.at(2) << "." << i_msg_receive.at(3);

                    // configure stack for this application
                    stackInit();
                }
                else
                {
                    std::cout << "EZSP version Not supported !" << std::endl;
                }
            }
            else
            {
                std::cout << "EZSP Response call for another command !!! How it is possible : " << i_cmd << std::endl;
            }
        });
    }
}

void CAppDemo::ashRxMessage( std::vector<uint8_t> i_message ) {
    std::stringstream bufDump;

    for (size_t i =0; i<i_message.size(); i++) {
        bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_message[i]) << " ";
    }
    std::cout << "ashRxMessage : " << bufDump.str() << std::endl;    
}

void CAppDemo::ezspHandler( EEzspCmd i_cmd, std::vector<uint8_t> i_message ) {
    std::stringstream bufDump;

    for (size_t i =0; i<i_message.size(); i++) {
        bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_message[i]) << " ";
    }
    std::cout << "ezspHandler : " << bufDump.str() << std::endl;    
}


/**
 * 
 * PRIVATE FUNCTIONS
 *  
 */

void CAppDemo::stackInit()
{
  std::vector<uint8_t> l_payload;
  SEzspConfig l_config[] = {
    {EZSP_CONFIG_PACKET_BUFFER_COUNT,24},
    {EZSP_CONFIG_NEIGHBOR_TABLE_SIZE,16},
    {EZSP_CONFIG_APS_UNICAST_MESSAGE_COUNT,10},
    {EZSP_CONFIG_BINDING_TABLE_SIZE,0},
    {EZSP_CONFIG_ADDRESS_TABLE_SIZE,8},
    {EZSP_CONFIG_MULTICAST_TABLE_SIZE,8},
    {EZSP_CONFIG_ROUTE_TABLE_SIZE, 16},
    {EZSP_CONFIG_DISCOVERY_TABLE_SIZE, 8},
    {EZSP_CONFIG_STACK_PROFILE, 2},
    {EZSP_CONFIG_SECURITY_LEVEL, 5},
    {EZSP_CONFIG_MAX_HOPS, 30},
    {EZSP_CONFIG_MAX_END_DEVICE_CHILDREN, 6},
    {EZSP_CONFIG_INDIRECT_TRANSMISSION_TIMEOUT, 3000},
    {EZSP_CONFIG_END_DEVICE_POLL_TIMEOUT, 5},
    {EZSP_CONFIG_MOBILE_NODE_POLL_TIMEOUT, 20},
    {EZSP_CONFIG_RESERVED_MOBILE_CHILD_ENTRIES, 0},
    {EZSP_CONFIG_TX_POWER_MODE, 0},
    {EZSP_CONFIG_DISABLE_RELAY, 0},
    {EZSP_CONFIG_TRUST_CENTER_ADDRESS_CACHE_SIZE, 0},
    {EZSP_CONFIG_SOURCE_ROUTE_TABLE_SIZE, 0},
    {EZSP_CONFIG_END_DEVICE_POLL_TIMEOUT_SHIFT, 6},
    {EZSP_CONFIG_FRAGMENT_WINDOW_SIZE, 0},
    {EZSP_CONFIG_FRAGMENT_DELAY_MS, 0},
    {EZSP_CONFIG_KEY_TABLE_SIZE, 12},
    {EZSP_CONFIG_APS_ACK_TIMEOUT, (50*30)+100},
    {EZSP_CONFIG_BEACON_JITTER_DURATION, 3},
    {EZSP_CONFIG_END_DEVICE_BIND_TIMEOUT, 60},
    {EZSP_CONFIG_PAN_ID_CONFLICT_REPORT_THRESHOLD, 1},
    {EZSP_CONFIG_REQUEST_KEY_TIMEOUT, 0},
    //*{EZSP_CONFIG_CERTIFICATE_TABLE_SIZE, 1},*/
    {EZSP_CONFIG_APPLICATION_ZDO_FLAGS, 0},
    {EZSP_CONFIG_BROADCAST_TABLE_SIZE, 15},
    {EZSP_CONFIG_MAC_FILTER_TABLE_SIZE, 0},
    {EZSP_CONFIG_SUPPORTED_NETWORKS, 1},
    {EZSP_CONFIG_SEND_MULTICASTS_TO_SLEEPY_ADDRESS, 0},
    {EZSP_CONFIG_ZLL_GROUP_ADDRESSES, 0},
    /*{EZSP_CONFIG_ZLL_RSSI_THRESHOLD, -128},*/
    {EZSP_CONFIG_MTORR_FLOW_CONTROL, 1},
    {EZSP_CONFIG_RETRY_QUEUE_SIZE, 8},
    {EZSP_CONFIG_NEW_BROADCAST_ENTRY_THRESHOLD, 10},
    {EZSP_CONFIG_TRANSIENT_KEY_TIMEOUT_S, 300},
    {EZSP_CONFIG_BROADCAST_MIN_ACKS_NEEDED, 1},
    {EZSP_CONFIG_TC_REJOINS_USING_WELL_KNOWN_KEY_TIMEOUT_S, 600},
  };
  #define l_config_size (sizeof(l_config)/sizeof(SEzspConfig))


  SEzspPolicy l_policy[] = {
    {EZSP_TRUST_CENTER_POLICY,EZSP_ALLOW_PRECONFIGURED_KEY_JOINS},
    {EZSP_MESSAGE_CONTENTS_IN_CALLBACK_POLICY,EZSP_MESSAGE_TAG_ONLY_IN_CALLBACK},
    {EZSP_BINDING_MODIFICATION_POLICY,EZSP_CHECK_BINDING_MODIFICATIONS_ARE_VALID_ENDPOINT_CLUSTERS},
    {EZSP_POLL_HANDLER_POLICY,EZSP_POLL_HANDLER_IGNORE},
  };
  #define l_policy_size (sizeof(l_policy)/sizeof(SEzspPolicy))

  // set config
  for(uint8_t loop=0; loop<l_config_size; loop++ )
  {
    l_payload.clear();
    l_payload.push_back(l_config[loop].id);
    l_payload.push_back(l_config[loop].value&0xFF);
    l_payload.push_back(l_config[loop].value>>8);
    dongle.sendCommand(EZSP_SET_CONFIGURATION_VALUE, l_payload);
  }

  // set policy
  for(uint8_t loop=0; loop<l_policy_size; loop++ )
  {
    l_payload.clear();
    l_payload.push_back(l_policy[loop].id);
    l_payload.push_back(l_policy[loop].decision);
    dongle.sendCommand(EZSP_SET_POLICY, l_payload);
  }

  // add endpoint
  l_payload.clear();
  l_payload.push_back(1); // ep number
  l_payload.push_back(0x04U); // profile id
  l_payload.push_back(0x01U);
  l_payload.push_back(0x07U); // device id
  l_payload.push_back(0x00U);
  l_payload.push_back(0); // flags
  l_payload.push_back(1); // in cluster count
  l_payload.push_back(1); // out cluster count
  l_payload.push_back(0); // in cluster
  l_payload.push_back(0);
  l_payload.push_back(0); // out cluster
  l_payload.push_back(0);
  dongle.sendCommand(EZSP_ADD_ENDPOINT, l_payload, [&](EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive){
        
        if( EZSP_ADD_ENDPOINT == i_cmd )
        {
            // configuration finished, initialize zigbee pro stack
            dongle.sendCommand(EZSP_NETWORK_INIT);
        }
        else
        {
            std::cout << "EZSP Response call for another command !!! How it is possible : " << i_cmd << std::endl;
        }
    });
}