

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <map>

#include "mainEzspTest.h"
#include "../domain/ezsp-protocol/get-network-parameters-response.h"
#include "../domain/ezsp-protocol/struct/ember-key-struct.h"
#include "../domain/ezsp-protocol/ezsp-enum.h"


CAppDemo::CAppDemo(IUartDriver *uartDriver) : dongle(this) { 
    setAppState(APP_NOT_INIT);
    // uart
    if( dongle.open(uartDriver) )
    {
        std::cout << "CAppDemo open success !" << std::endl;
        setAppState(APP_INIT_IN_PROGRESS);
    }
}

void CAppDemo::dongleState( EDongleState i_state )
{
    std::cout << "CAppDemo::dongleState : " << i_state << std::endl;

    if( DONGLE_READY == i_state )
    {
        if( APP_INIT_IN_PROGRESS == app_state )
        {
            dongleInit();
        }
    }
    else if( DONGLE_REMOVE == i_state )
    {
        // \todo manage this !
    }
}

void CAppDemo::ashRxMessage( std::vector<uint8_t> i_message ) {
    // Callback to catch incomming ash raw message, for debug purpose
/*    
    std::stringstream bufDump;

    for (size_t i =0; i<i_message.size(); i++) {
        bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_message[i]) << " ";
    }
    std::cout << "ashRxMessage : " << bufDump.str() << std::endl;    
*/    
}

void CAppDemo::ezspHandler( EEzspCmd i_cmd, std::vector<uint8_t> i_message ) {
    std::cout << "CAppDemo::ezspHandler " << CEzspEnum::EEzspCmdToString(i_cmd) << std::endl;

    if( EZSP_STACK_STATUS_HANDLER == i_cmd )
    {
        std::cout << "CAppDemo::ezspHandler : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_message.at(0))) << std::endl;
        setAppState(APP_READY);

        // we open network, so we can enter new devices
        OpenNetwork( 10 );

        // we retrieve network information and key
        std::vector<uint8_t> l_payload;
        dongle.sendCommand(EZSP_GET_NETWORK_PARAMETERS, l_payload, [&](EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive){
            CGetNetworkParamtersResponse l_rsp(i_msg_receive);
            std::cout << l_rsp.String() << std::endl;
        });
        l_payload.push_back(EMBER_CURRENT_NETWORK_KEY);
        dongle.sendCommand(EZSP_GET_KEY, l_payload, [&](EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive){
            EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));
            i_msg_receive.erase(i_msg_receive.begin());
            CEmberKeyStruct l_rsp(i_msg_receive);
            std::cout << "EZSP_GET_KEY status : " << CEzspEnum::EEmberStatusToString(l_status) << ", " << l_rsp.String() << std::endl;
        });

        // start discover of existing product inside network
        startDiscoverProduct();
    }
    else
    {
        std::stringstream bufDump;

        for (size_t i =0; i<i_message.size(); i++) {
            bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_message[i]) << " ";
        }
        std::cout << "CAppDemo::ezspHandler : " << bufDump.str() << std::endl;    
    }
}


/**
 * 
 * PRIVATE FUNCTIONS
 *  
 */

/**
 * utility function can managed error state
 */
void CAppDemo::setAppState( EAppState i_state )
{
    app_state = i_state;

    const std::map<EAppState,std::string> MyEnumStrings {
        { APP_NOT_INIT, "APP_NOT_INIT" },
        { APP_READY, "APP_READY" },
        { APP_ERROR, "APP_ERROR" },
        { APP_INIT_IN_PROGRESS, "APP_INIT_IN_PROGRESS" },
        { APP_FORM_NWK_IN_PROGRESS, "APP_FORM_NWK_IN_PROGRESS" },
    };

    auto   it  = MyEnumStrings.find(app_state);
    std::string error_str = it == MyEnumStrings.end() ? "OUT_OF_RANGE" : it->second;
    std::cout << "APP State change : " << error_str << std::endl;
}

void CAppDemo::dongleInit()
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
                std::stringstream bufDump;

                // prtocol
                bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[0]) << ".";

                // type
                bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[1]) << ".";

                // version
                uint16_t l_version = i_msg_receive[2] + (i_msg_receive[3]<<8);
                bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(l_version);

                std::cout << "Stack version : " << bufDump.str() << std::endl;                    

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

void CAppDemo::stackInit()
{
  std::vector<uint8_t> l_payload;

  SEzspConfig l_config[] = {
    {EZSP_CONFIG_NEIGHBOR_TABLE_SIZE,32},
    {EZSP_CONFIG_APS_UNICAST_MESSAGE_COUNT,10},
    {EZSP_CONFIG_BINDING_TABLE_SIZE,0},
    {EZSP_CONFIG_ADDRESS_TABLE_SIZE,64},
    {EZSP_CONFIG_MULTICAST_TABLE_SIZE,8},
    {EZSP_CONFIG_ROUTE_TABLE_SIZE, 32},
    {EZSP_CONFIG_DISCOVERY_TABLE_SIZE, 16},
    {EZSP_CONFIG_STACK_PROFILE, 2},
    {EZSP_CONFIG_SECURITY_LEVEL, 5},
    {EZSP_CONFIG_MAX_HOPS, 15},
    {EZSP_CONFIG_MAX_END_DEVICE_CHILDREN, 32}, // define number of sleepy end device directly attached to dongle
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
    {EZSP_CONFIG_PACKET_BUFFER_COUNT,0xFF}, // use all remain memory for in/out radio packets
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
    l_payload.push_back(static_cast<uint8_t>(l_config[loop].value&0xFF));
    l_payload.push_back(static_cast<uint8_t>(l_config[loop].value>>8));
    //std::cout << "CAppDemo::stackInit : EZSP_SET_CONFIGURATION_VALUE : " << unsigned(l_config[loop].id) << std::endl;
    dongle.sendCommand(EZSP_SET_CONFIGURATION_VALUE, l_payload, [&](EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive){ 
        if( 0 != i_msg_receive.at(0) ) {
            std::cout << "CAppDemo::stackInit : EZSP_SET_CONFIGURATION_VALUE RSP : " << unsigned(i_msg_receive.at(0)) << std::endl;
        }
    });
  }

  // set policy
  for(uint8_t loop=0; loop<l_policy_size; loop++ )
  {
    l_payload.clear();
    l_payload.push_back(l_policy[loop].id);
    l_payload.push_back(l_policy[loop].decision);
    //std::cout << "CAppDemo::stackInit : EZSP_SET_POLICY : " << unsigned(l_policy[loop].id) << std::endl;
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
        
        // configuration finished, initialize zigbee pro stack
        std::cout << "CAppDemo::stackInit Call EZSP_NETWORK_INIT" << std::endl;
        dongle.sendCommand(EZSP_NETWORK_INIT, std::vector<uint8_t>(), [&](EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive){
            // configuration finished, initialize zigbee pro stack
            std::cout << "CAppDemo::stackInit Call EZSP_NETWORK_STATE" << std::endl;
            dongle.sendCommand(EZSP_NETWORK_STATE, std::vector<uint8_t>(), [&](EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive){
                std::cout << "CAppDemo::stackInit Return EZSP_NETWORK_STATE : " << unsigned(i_msg_receive.at(0)) << std::endl;
                if( EMBER_NO_NETWORK == i_msg_receive.at(0) )
                {
                    // we decide to create an HA1.2 network
                    std::cout << "CAppDemo::stackInit Call formHaNetwork" << std::endl;
                    formHaNetwork();
                }
            });
        });
    });
}

void CAppDemo::formHaNetwork()
{
    if( APP_INIT_IN_PROGRESS == app_state )
    {
        // set HA policy
        std::vector<uint8_t> payload;
        uint16_t l_security_bitmak = 0;
        std::srand(std::time(nullptr));

        payload.push_back(EZSP_TRUST_CENTER_POLICY); // EZSP_TRUST_CENTER_POLICY
        payload.push_back(0x01); // EZSP_ALLOW_PRECONFIGURED_KEY_JOINS
        dongle.sendCommand( EZSP_SET_POLICY, payload );

        payload.clear();
        payload.push_back(EZSP_TC_KEY_REQUEST_POLICY); // EZSP_TC_KEY_REQUEST_POLICY
        payload.push_back(0x50); // EZSP_DENY_TC_KEY_REQUESTS
        dongle.sendCommand( EZSP_SET_POLICY, payload );

        // set initial security state
        // EMBER_HAVE_PRECONFIGURED_KEY
        l_security_bitmak |= 0x0100;

        // EMBER_HAVE_NETWORK_KEY
        l_security_bitmak |= 0x0200;

        // EMBER_REQUIRE_ENCRYPTED_KEY
        l_security_bitmak |= 0x0800;

        // EMBER_TRUST_CENTER_GLOBAL_LINK_KEY
        l_security_bitmak |= 0x0004;

        payload.clear();
        // security bitmask
        payload.push_back(static_cast<uint8_t>(l_security_bitmak&0xFF));
        payload.push_back(static_cast<uint8_t>((l_security_bitmak>>8)&0xFF));
        // tc key : HA Key
        payload.push_back(0x5A);
        payload.push_back(0x69);
        payload.push_back(0x67);
        payload.push_back(0x42);
        payload.push_back(0x65);
        payload.push_back(0x65);
        payload.push_back(0x41);
        payload.push_back(0x6C);
        payload.push_back(0x6C);
        payload.push_back(0x69);
        payload.push_back(0x61);
        payload.push_back(0x6E);
        payload.push_back(0x63);
        payload.push_back(0x65);
        payload.push_back(0x30);
        payload.push_back(0x39);
        //  network key : random
        for( uint8_t loop=0; loop<16; loop++ ){ payload.push_back( static_cast<uint8_t>(std::rand()&0xFF )); }
        // key sequence number
        payload.push_back(0U);
        // eui trust center : not used
        for( uint8_t loop=0; loop<8; loop++ ){ payload.push_back( 0U ); }

        // call
        dongle.sendCommand(EZSP_SET_INITIAL_SECURITY_STATE, payload, [&](EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive){
            std::string status_str = CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0)));
            std::cout << "CAppDemo::formHaNetwork : EZSP_SET_INITIAL_SECURITY_STATE status : " << status_str << std::endl;
            if( EMBER_SUCCESS == i_msg_receive.at(0) )
            {
                CEmberNetworkParameters payload;

                payload.setPanId(static_cast<uint16_t>(std::rand()&0xFFFF));
                payload.setRadioTxPower(3);
                payload.setRadioChannel(11);
                payload.setJoinMethod(EMBER_USE_MAC_ASSOCIATION);

                dongle.sendCommand(EZSP_FORM_NETWORK, payload.getRaw(), [&](EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive){
                    std::string status_str = CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0)));
                    std::cout << "CAppDemo::formHaNetwork : EZSP_FORM_NETWORK status : " << status_str << std::endl;
                });
            }
            else
            {
                setAppState(APP_ERROR);
            }
        });

        //set new state
        setAppState(APP_FORM_NWK_IN_PROGRESS);
    }
}

/**
 * @brief OpenNetwork : open current network
 * @param i_timeout : timeout for open network in second
 * @return true if action is possible
 */
bool CAppDemo::OpenNetwork( uint8_t i_timeout )
{
    bool lo_success = false;

    // check validity of command
    if( APP_READY == app_state )
    {
        std::vector<uint8_t> i_payload;

        i_payload.push_back(i_timeout);
        std::cout << "CAppDemo::OpenNetwork Call EZSP_PERMIT_JOINING" << std::endl;
        dongle.sendCommand(EZSP_PERMIT_JOINING, i_payload, [&](EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive){
            std::cout << "CAppDemo::OpenNetwork EZSP_PERMIT_JOINING return : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0))) << std::endl;
        });

        // use zdp frame
        CZigBeeMsg l_msg;
        std::vector<uint8_t> l_payload;

        l_payload.push_back(i_timeout);
        l_payload.push_back(1);

        l_msg.SetZdo( 0x0036, l_payload );

        SendBroadcast( E_OUT_MSG_BR_DEST_ALL_DEVICES, 0, l_msg );

        lo_success = true;
    }

    return lo_success;
}

/**
 * @brief CloseNetwork : open current network
 * @return true if action is possible
 */
bool CAppDemo::CloseNetwork( void )
{
    bool lo_success = false;

    // check validity of command
    if( APP_READY == app_state )
    {
        std::vector<uint8_t> i_payload;

        i_payload.push_back(0);
        dongle.sendCommand(EZSP_PERMIT_JOINING,i_payload);

        // use zdp frame
        CZigBeeMsg l_msg;
        std::vector<uint8_t> l_payload;

        l_payload.push_back(0);
        l_payload.push_back(1);

        l_msg.SetZdo( 0x0036, l_payload );

        SendBroadcast( E_OUT_MSG_BR_DEST_ALL_DEVICES, 0, l_msg );

        lo_success = true;
    }

  return lo_success;
}

/**
 * @brief SendBroadcast : send broadcast zigbee message
 * @param i_destination : type of node concern by broadcast
 * @param radius : The message will be delivered to all nodes within radius hops of the sender.
 *                  A radius of zero is converted to EMBER_MAX_HOPS.
 * @param i_msg : meassge to send
 */
void CAppDemo::SendBroadcast( EOutBroadcastDestination i_destination, uint8_t i_radius, CZigBeeMsg i_msg)
{
  // COutZbMessage l_zb_msg = COutZbMessage();
  // l_zb_msg.SetBroadcastMessage( i_destination, i_radius, i_msg );

    std::vector<uint8_t> l_payload;
    std::vector<uint8_t> l_zb_msg = i_msg.Get();

    // destination
    l_payload.push_back( static_cast<uint8_t>(i_destination&0xFF) );
    l_payload.push_back( static_cast<uint8_t>((i_destination>>8)&0xFF) );

    // aps frame
    std::vector<uint8_t> v_tmp = i_msg.GetAps()->GetEmberAPS();
    l_payload.insert(l_payload.end(), v_tmp.begin(), v_tmp.end());

    // radius
    l_payload.push_back( i_radius );
 
    // message tag : not used for this simplier demo
    l_payload.push_back( 0 );

    // message length
    l_payload.push_back( static_cast<uint8_t>(l_zb_msg.size()) );

    // message content
    l_payload.insert(l_payload.end(), l_zb_msg.begin(), l_zb_msg.end());


    dongle.sendCommand(EZSP_SEND_BROADCAST, l_payload, [&](EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive){
        std::cout << "CAppDemo::SendBroadcast EZSP_SEND_BROADCAST return : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0))) << std::endl;
    });
}

void CAppDemo::startDiscoverProduct()
{
    // pour l'exemple on ne lit que la table enfant du dongle, on assume qu'il n'y a pas d'autre routeur dans le réseau
    // lire table enfant du dongle
    std::vector<uint8_t> l_param;
    child_idx = 0;
    l_param.push_back(child_idx);
    dongle.sendCommand(EZSP_GET_CHILD_DATA, l_param, [&](EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive){
        std::cout << "EZSP_GET_CHILD_DATA return  at index : " << unsigned(child_idx) << ", status : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0))) << std::endl;
        if( EMBER_SUCCESS == i_msg_receive.at(0) )
        {
            // todo ecrire une class structure EmberChildData pour décoder la trame

            // appeler la fonction de nouveau produit

            // lire l'entrée suivante
            // ATTENTION Fonction ré-entrante !!!!
        }
    });
    
}