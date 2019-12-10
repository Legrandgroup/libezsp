/**
 * @file zigbee-networking.cpp
 */

#include <ctime>

#include "../byte-manip.h"

#include "zigbee-networking.h"

#include "domain/ezsp-protocol/get-network-parameters-response.h"
#include "domain/ezsp-protocol/struct/ember-key-struct.h"
#include "domain/ezsp-protocol/struct/ember-child-data-struct.h"

#include "spi/GenericLogger.h"


CZigbeeNetworking::CZigbeeNetworking( CEzspDongle &i_dongle, CZigbeeMessaging &i_zb_messaging ) :
    dongle(i_dongle),
    zb_messaging(i_zb_messaging),
    child_idx(0),
    discoverCallbackFct(nullptr),
    form_channel(DEFAULT_RADIO_CHANNEL)
{
    dongle.registerObserver(this);
}

void CZigbeeNetworking::handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive )
{
    // clogD << "CZigbeeNetworking::handleEzspRxMessage : " << CEzspEnum::EEzspCmdToString(i_cmd) << std::endl;

    switch( i_cmd )
    {
        case EZSP_PERMIT_JOINING:
        {
            clogD << "EZSP_PERMIT_JOINING return status : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0))) << std::endl;
        }
        break;

        case EZSP_SEND_BROADCAST:
        {
            clogD << "EZSP_SEND_BROADCAST return status : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0))) << std::endl;
        }
        break;
        case EZSP_GET_CHILD_DATA:
        {
            clogD << "EZSP_GET_CHILD_DATA return  at index : " << unsigned(child_idx) << ", status : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0))) << std::endl;
            if( EMBER_SUCCESS == i_msg_receive.at(0) )
            {
                i_msg_receive.erase(i_msg_receive.begin());
                CEmberChildDataStruct l_rsp(i_msg_receive);
                clogD << l_rsp.String() << std::endl;

                // appeler la fonction de nouveau produit
                if( nullptr != discoverCallbackFct )
                {
                    discoverCallbackFct(l_rsp.getType(), l_rsp.getEui64(), l_rsp.getId());
                }

                // lire l'entrée suivante
                std::vector<uint8_t> l_param;
                child_idx++;
                l_param.push_back(child_idx);
                dongle.sendCommand(EZSP_GET_CHILD_DATA, l_param);
            }
        }
        break;
        case EZSP_SET_INITIAL_SECURITY_STATE:
        {
            clogD << "EZSP_SET_INITIAL_SECURITY_STATE status : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0))) << std::endl;
            if( EMBER_SUCCESS == i_msg_receive.at(0) )
            {
                CEmberNetworkParameters payload;

                payload.setPanId(static_cast<uint16_t>(std::rand()&0xFFFF));
                payload.setRadioTxPower(3);
                payload.setRadioChannel(form_channel);
                payload.setJoinMethod(EMBER_USE_MAC_ASSOCIATION);

                dongle.sendCommand(EZSP_FORM_NETWORK, payload.getRaw());
            }
        }
        break;
        case EZSP_SET_CONFIGURATION_VALUE:
        {
            if( 0 != i_msg_receive.at(0) ) {
                clogD << "EZSP_SET_CONFIGURATION_VALUE RSP : " << unsigned(i_msg_receive.at(0)) << std::endl;
            }
        }
        break;
        case EZSP_ADD_ENDPOINT:
        {
            // configuration finished, initialize zigbee pro stack
            clogD << "Call EZSP_NETWORK_INIT" << std::endl;
            dongle.sendCommand(EZSP_NETWORK_INIT);
        }
        break;
        case EZSP_NETWORK_INIT:
        {
            // initialize zigbee pro stack finished, get the current network state
            clogD << "Call EZSP_NETWORK_STATE" << std::endl;
            dongle.sendCommand(EZSP_NETWORK_STATE);
        }
        break;
        case EZSP_FORM_NETWORK:
        {
            clogD << "EZSP_FORM_NETWORK status : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0))) << std::endl;
        }
        break;
        case EZSP_LEAVE_NETWORK:
        {
            clogD << "EZSP_LEAVE_NETWORK status : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0))) << std::endl;
        }
        break;

        default:
        break;
    }

}

void CZigbeeNetworking::stackInit(const std::vector<SEzspConfig>& l_config, const std::vector<SEzspPolicy>& l_policy)
{
  std::vector<uint8_t> l_payload;

  // set config
  for(auto it : l_config)
  {
    l_payload.clear();
    l_payload.push_back(it.id);
    l_payload.push_back(u16_get_lo_u8(it.value));
    l_payload.push_back(u16_get_hi_u8(it.value));
    //clogD << "EZSP_SET_CONFIGURATION_VALUE : " << unsigned(l_config[loop].id) << std::endl;
    dongle.sendCommand(EZSP_SET_CONFIGURATION_VALUE, l_payload);
  }

  // set policy
  for(auto it : l_policy)
  {
    l_payload.clear();
    l_payload.push_back(it.id);
    l_payload.push_back(it.decision);
    //clogD << "EZSP_SET_POLICY : " << unsigned(l_policy[loop].id) << std::endl;
    dongle.sendCommand(EZSP_SET_POLICY, l_payload);
  }

  // add endpoint 1 : gateway device
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
  dongle.sendCommand(EZSP_ADD_ENDPOINT, l_payload);

  // add endpoint 242 : green power
  l_payload.clear();
  l_payload.push_back(242); // ep number
  l_payload.push_back(0x0EU); // profile id
  l_payload.push_back(0xA1U);
  l_payload.push_back(0x64U); // device id
  l_payload.push_back(0x00U);
  l_payload.push_back(0); // flags
  l_payload.push_back(1); // in cluster count
  l_payload.push_back(1); // out cluster count
  l_payload.push_back(0x21); // in cluster
  l_payload.push_back(0);
  l_payload.push_back(0x21); // out cluster
  l_payload.push_back(0);
  dongle.sendCommand(EZSP_ADD_ENDPOINT, l_payload);
}

void CZigbeeNetworking::formHaNetwork(uint8_t channel)
{
    // set HA policy
    std::vector<uint8_t> payload;
    uint16_t l_security_bitmak = 0;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

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
    dongle.sendCommand(EZSP_SET_INITIAL_SECURITY_STATE, payload);

    // set parameter for next step
    form_channel = channel;
}

void CZigbeeNetworking::openNetwork(uint8_t i_timeout)
{
    std::vector<uint8_t> i_payload;

    i_payload.push_back(i_timeout);
    dongle.sendCommand(EZSP_PERMIT_JOINING, i_payload);

    // use zdp frame
    CZigBeeMsg l_msg;
    std::vector<uint8_t> l_payload;

    l_payload.push_back(i_timeout);
    l_payload.push_back(1);

    l_msg.SetZdo( 0x0036, l_payload );

    zb_messaging.SendBroadcast( E_OUT_MSG_BR_DEST_ALL_DEVICES, 0, l_msg );
}

void CZigbeeNetworking::closeNetwork()
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

    zb_messaging.SendBroadcast( E_OUT_MSG_BR_DEST_ALL_DEVICES, 0, l_msg );
}

void CZigbeeNetworking::leaveNetwork()
{
    dongle.sendCommand(EZSP_LEAVE_NETWORK);
}

void CZigbeeNetworking::startDiscoverProduct(std::function<void (EmberNodeType i_type, EmberEUI64 i_eui64, EmberNodeId i_id)> i_discoverCallbackFct)
{
    // pour l'exemple on ne lit que la table enfant du dongle, on assume qu'il n'y a pas d'autre routeur dans le réseau
    // lire table enfant du dongle
    std::vector<uint8_t> l_param;
    child_idx = 0;
    l_param.push_back(child_idx);
    dongle.sendCommand(EZSP_GET_CHILD_DATA, l_param);

    discoverCallbackFct = i_discoverCallbackFct;
}
