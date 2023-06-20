/**
 * @file zigbee-networking.cpp
 */

#include <climits>

#include "ezsp/byte-manip.h"

#include "ezsp/zigbee-tools/zigbee-networking.h"

#include "ezsp/ezsp-protocol/get-network-parameters-response.h"
#include "ezsp/ezsp-protocol/struct/ember-key-struct.h"
#include "ezsp/ezsp-protocol/struct/ember-child-data-struct.h"

#include "spi/ILogger.h"

static std::random_device g_rd;

using NSEZSP::CZigbeeNetworking;

CZigbeeNetworking::CZigbeeNetworking( CEzspDongle &i_dongle, CZigbeeMessaging &i_zb_messaging ) :
	random_generator(g_rd()),
	dongle(i_dongle),
	zb_messaging(i_zb_messaging),
	child_idx(0),
	discoverCallbackFct(nullptr),
	form_channel(DEFAULT_RADIO_CHANNEL) {
	dongle.registerObserver(this);
}

void CZigbeeNetworking::handleEzspRxMessage(EEzspCmd i_cmd, NSSPI::ByteBuffer i_msg_receive) {
	// clogD << "CZigbeeNetworking::handleEzspRxMessage : " << CEzspEnum::EEzspCmdToString(i_cmd) << std::endl;

	switch( i_cmd ) {
	case EZSP_PERMIT_JOINING: {
		clogD << "EZSP_PERMIT_JOINING return status : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0))) << std::endl;
	}
	break;

	case EZSP_SEND_BROADCAST: {
		clogD << "EZSP_SEND_BROADCAST return status : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0))) << std::endl;
	}
	break;
	case EZSP_GET_CHILD_DATA: {
		clogD << "EZSP_GET_CHILD_DATA return  at index : " << unsigned(child_idx) << ", status : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0))) << std::endl;
		if( EMBER_SUCCESS == i_msg_receive.at(0) ) {
			i_msg_receive.erase(i_msg_receive.begin());
			CEmberChildDataStruct l_rsp(i_msg_receive);
			clogD << l_rsp.String() << std::endl;

			// appeler la fonction de nouveau produit
			if( nullptr != discoverCallbackFct ) {
				discoverCallbackFct(l_rsp.getType(), l_rsp.getEui64(), l_rsp.getId());
			}

			// lire l'entrée suivante
			this->child_idx++;
			dongle.sendCommand(EZSP_GET_CHILD_DATA, { this->child_idx });
		}
	}
	break;
	case EZSP_SET_INITIAL_SECURITY_STATE: {
		clogD << "EZSP_SET_INITIAL_SECURITY_STATE status : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0))) << std::endl;
		std::uniform_int_distribution<uint16_t> dis(0, USHRT_MAX);
		if( EMBER_SUCCESS == i_msg_receive.at(0) ) {
			CEmberNetworkParameters payload;

			payload.setPanId(dis(this->random_generator)&0xFFFF);
			payload.setRadioTxPower(3);
			payload.setRadioChannel(form_channel);
			payload.setJoinMethod(EMBER_USE_MAC_ASSOCIATION);

			dongle.sendCommand(EZSP_FORM_NETWORK, payload.getRaw());
		}
	}
	break;
	case EZSP_SET_CONFIGURATION_VALUE: {
		if( 0 != i_msg_receive.at(0) ) {
			clogD << "EZSP_SET_CONFIGURATION_VALUE RSP : " << unsigned(i_msg_receive.at(0)) << std::endl;
		}
	}
	break;
	case EZSP_ADD_ENDPOINT: {
		// configuration finished, initialize zigbee pro stack
		clogD << "Call EZSP_NETWORK_INIT" << std::endl;
		dongle.sendCommand(EZSP_NETWORK_INIT);
	}
	break;
	case EZSP_NETWORK_INIT: {
		// initialize zigbee pro stack finished, get the current network state
		clogD << "Call EZSP_NETWORK_STATE" << std::endl;
		dongle.sendCommand(EZSP_NETWORK_STATE);
	}
	break;
	case EZSP_FORM_NETWORK: {
		clogD << "EZSP_FORM_NETWORK status : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0))) << std::endl;
	}
	break;
	case EZSP_JOIN_NETWORK: {
		EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));
		clogD << "EZSP_JOIN_NETWORK status : " << CEzspEnum::EEmberStatusToString(l_status) << "\n";
	}
	break;
	case EZSP_LEAVE_NETWORK: {
		clogD << "EZSP_LEAVE_NETWORK status : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(0))) << std::endl;
	}
	break;

	default:
		break;
	}

}

void CZigbeeNetworking::stackInit(const std::vector<SEzspConfig>& l_config, const std::vector<SEzspPolicy>& l_policy) {
	// set config
	for(auto it : l_config) {
		//clogD << "EZSP_SET_CONFIGURATION_VALUE : " << unsigned(l_config[loop].id) << std::endl;
		dongle.sendCommand(EZSP_SET_CONFIGURATION_VALUE, { it.id, u16_get_lo_u8(it.value), u16_get_hi_u8(it.value) } );
	}

	// set policy
	for(auto it : l_policy) {
		//clogD << "EZSP_SET_POLICY : " << unsigned(l_policy[loop].id) << std::endl;
		dongle.sendCommand(EZSP_SET_POLICY, { it.id, it.decision });
	}

	{
		NSSPI::ByteBuffer l_payload;
		// add endpoint 1 : gateway device
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
	}

	{
		NSSPI::ByteBuffer l_payload;
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
}

void CZigbeeNetworking::setNetworkToJoin(NSEZSP::CEmberNetworkParameters& nwkParams) {
	this->joinNwkParams = nwkParams;
}

void CZigbeeNetworking::joinNetwork(NSEZSP::CEmberNetworkParameters& nwkParams) {
	clogD << "Running " << __func__ << "()\n";
	NSSPI::ByteBuffer payload;
	payload.push_back(EMBER_ROUTER);
	payload.append(nwkParams.getRaw());
	this->dongle.sendCommand(EZSP_JOIN_NETWORK, payload);
}

void CZigbeeNetworking::joinNetwork() {
	this->joinNetwork(this->joinNwkParams);
}

void CZigbeeNetworking::formHaNetwork(uint8_t channel) {
	// set HA policy
	dongle.sendCommand( EZSP_SET_POLICY, { EZSP_TRUST_CENTER_POLICY, 0x01U });

	dongle.sendCommand( EZSP_SET_POLICY, { EZSP_TC_KEY_REQUEST_POLICY, 0x50U /* EZSP_DENY_TC_KEY_REQUESTS */});

	uint16_t l_security_bitmak = 0;

	// set initial security state
	// EMBER_HAVE_PRECONFIGURED_KEY
	l_security_bitmak |= 0x0100U;

	// EMBER_HAVE_NETWORK_KEY
	l_security_bitmak |= 0x0200U;

	// EMBER_REQUIRE_ENCRYPTED_KEY
	l_security_bitmak |= 0x0800U;

	// EMBER_TRUST_CENTER_GLOBAL_LINK_KEY
	l_security_bitmak |= 0x0004U;

	NSSPI::ByteBuffer payload;
	// security bitmask
	payload.push_back(u16_get_lo_u8(l_security_bitmak));
	payload.push_back(u16_get_hi_u8(l_security_bitmak));
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
	std::uniform_int_distribution<uint8_t> dis(0, 255);
	for( uint8_t loop=0; loop<16; loop++ ) {
		payload.push_back( dis(this->random_generator)&0xFF);
	}
	// key sequence number
	payload.push_back(0U);
	// eui trust center : not used
	for( uint8_t loop=0; loop<8; loop++ ) {
		payload.push_back( 0U );
	}

	// call
	dongle.sendCommand(EZSP_SET_INITIAL_SECURITY_STATE, payload);

	// set parameter for next step
	form_channel = channel;
}

void CZigbeeNetworking::openNetwork(uint8_t i_timeout) {
	dongle.sendCommand(EZSP_SET_POLICY, { EZSP_TC_KEY_REQUEST_POLICY, 0x51U /* EZSP_ALLOW_TC_KEY_REQUESTS_AND_SEND_CURRENT_KEY */});
	dongle.sendCommand(EZSP_PERMIT_JOINING, { i_timeout });

	// Send Permit_Joining_Req
	CZigBeeMsg l_msg;
	l_msg.SetZdo( 0x0036, { i_timeout, 1 });
	zb_messaging.SendBroadcast( E_OUT_MSG_BR_DEST_ALL_DEVICES, 0, l_msg );
}

void CZigbeeNetworking::closeNetwork() {
	dongle.sendCommand( EZSP_PERMIT_JOINING, { 0x00U } );
	dongle.sendCommand( EZSP_SET_POLICY, { EZSP_TC_KEY_REQUEST_POLICY, 0x50U /* EZSP_DENY_TC_KEY_REQUESTS */});

	// use zdp frame
	CZigBeeMsg l_msg;

	l_msg.SetZdo( 0x0036, { 0x00U, 0x01U });

	zb_messaging.SendBroadcast( E_OUT_MSG_BR_DEST_ALL_DEVICES, 0, l_msg );
}

void CZigbeeNetworking::leaveNetwork() {
	dongle.sendCommand(EZSP_LEAVE_NETWORK);
}
