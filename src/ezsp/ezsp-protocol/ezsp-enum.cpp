/**
 * @file ezsp-enum.cpp
 *
 * @brief Various EZSP-constants from the EZSP specs and related manipulation functions
 */
#include <map>

#include "ezsp/ezsp-protocol/ezsp-enum.h"

using NSEZSP::CEzspEnum;

std::string CEzspEnum::EmberNodeTypeToString( EmberNodeType in ) {
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

std::string CEzspEnum::EEmberStatusToString( EEmberStatus in ) {
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
		{ EMBER_NO_NETWORK_KEY_RECEIVED, "EMBER_NO_NETWORK_KEY_RECEIVED" },
		{ UNDOCUMENTED_WRONG_MIC_FOR_SOURCE_ID, "UNDOCUMENTED_WRONG_MIC_FOR_SOURCE_ID" },
		{ UNDOCUMENTED_UNKNOWN_KEY_FOR_SOURCE_ID, "UNDOCUMENTED_UNKNOWN_KEY_FOR_SOURCE_ID" },
	};
	auto   it  = MyEnumStrings.find(in);
	return it == MyEnumStrings.end() ? "OUT_OF_RANGE : " + std::to_string(in) : it->second;
}

std::string CEzspEnum::EmberJoinMethodToString( EmberJoinMethod in ) {
	const std::map<EmberJoinMethod,std::string> MyEnumStrings {
		{ EMBER_USE_MAC_ASSOCIATION, "EMBER_USE_MAC_ASSOCIATION" },
		{ EMBER_USE_NWK_REJOIN, "EMBER_USE_NWK_REJOIN" },
		{ EMBER_USE_NWK_REJOIN_HAVE_NWK_KEY, "EMBER_USE_NWK_REJOIN_HAVE_NWK_KEY" },
		{ EMBER_USE_NWK_COMMISSIONING, "EMBER_USE_NWK_COMMISSIONING" }
	};
	auto   it  = MyEnumStrings.find(in);
	return it == MyEnumStrings.end() ? "OUT_OF_RANGE" : it->second;
}

std::string CEzspEnum::EEzspCmdToString( EEzspCmd in ) {
	const std::map<EEzspCmd,std::string> MyEnumStrings {
		/* Configuration Frames */
		{ EZSP_VERSION, "EZSP_VERSION" },
		{ EZSP_GET_CONFIGURATION_VALUE, "EZSP_GET_CONFIGURATION_VALUE" },
		{ EZSP_SET_CONFIGURATION_VALUE, "EZSP_SET_CONFIGURATION_VALUE" },
		{ EZSP_ADD_ENDPOINT, "EZSP_ADD_ENDPOINT" },
		{ EZSP_SET_POLICY, "EZSP_SET_POLICY" },
		{ EZSP_GET_POLICY, "EZSP_GET_POLICY" },
		{ EZSP_SEND_PAN_ID_UPDATE, "EZSP_SEND_PAN_ID_UPDATE" },
		{ EZSP_GET_VALUE, "EZSP_GET_VALUE" },
		{ EZSP_GET_EXTENDED_VALUE, "EZSP_GET_EXTENDED_VALUE" },
		{ EZSP_SET_VALUE, "EZSP_SET_VALUE" },
		{ EZSP_SET_GPIO_CURRENT_CONFIGURATION, "EZSP_SET_GPIO_CURRENT_CONFIGURATION" },
		{ EZSP_SET_GPIO_POWER_UP_DOWN_CONFIGURATION, "EZSP_SET_GPIO_POWER_UP_DOWN_CONFIGURATION" },
		{ EZSP_SET_GPIO_RADIO_POWER_MASK, "EZSP_SET_GPIO_RADIO_POWER_MASK" },
		{ EZSP_SET_CTUNE, "EZSP_SET_CTUNE" },
		{ EZSP_GET_CTUNE, "EZSP_GET_CTUNE" },
		/*  Utilities Frames */
		{ EZSP_NOP, "EZSP_NOP" },
		{ EZSP_ECHO, "EZSP_ECHO" },
		{ EZSP_INVALID_COMMAND, "EZSP_INVALID_COMMAND" },
		{ EZSP_CALLBACK, "EZSP_CALLBACK" },
		{ EZSP_NO_CALLBACKS, "EZSP_NO_CALLBACKS" },
		{ EZSP_SET_TOKEN, "EZSP_SET_TOKEN" },
		{ EZSP_GET_TOKEN, "EZSP_GET_TOKEN" },
		{ EZSP_GET_MFG_TOKEN, "EZSP_GET_MFG_TOKEN" },
		{ EZSP_SET_MFG_TOKEN, "EZSP_SET_MFG_TOKEN" },
		{ EZSP_STACK_TOKEN_CHANGED_HANDLER, "EZSP_STACK_TOKEN_CHANGED_HANDLER" },
		{ EZSP_GET_RANDOM_NUMBER, "EZSP_GET_RANDOM_NUMBER" },
		{ EZSP_SET_TIMER, "EZSP_SET_TIMER" },
		{ EZSP_GET_TIMER, "EZSP_GET_TIMER" },
		{ EZSP_TIMER_HANDLER, "EZSP_TIMER_HANDLER" },
		{ EZSP_DEBUG_WRITE, "EZSP_DEBUG_WRITE" },
		{ EZSP_READ_AND_CLEAR_COUNTERS, "EZSP_READ_AND_CLEAR_COUNTERS" },
		{ EZSP_READ_COUNTERS, "EZSP_READ_COUNTERS" },
		{ EZSP_COUNTER_ROLLOVER_HANDLER, "EZSP_COUNTER_ROLLOVER_HANDLER" },
		{ EZSP_DELAY_TEST, "EZSP_DELAY_TEST" },
		{ EZSP_GET_LIBRARY_STATUS, "EZSP_GET_LIBRARY_STATUS" },
		{ EZSP_GET_XNCP_INFO, "EZSP_GET_XNCP_INFO" },
		{ EZSP_CUSTOM_FRAME, "EZSP_CUSTOM_FRAME" },
		{ EZSP_CUSTOM_FRAME_HANDLER, "EZSP_CUSTOM_FRAME_HANDLER" },
		{ EZSP_GET_EUI64, "EZSP_GET_EUI64" },
		{ EZSP_GET_NODE_ID, "EZSP_GET_NODE_ID" },
		{ EZSP_GET_PHY_INTERFACE_COUNT, "EZSP_GET_PHY_INTERFACE_COUNT" },
		{ EZSP_GET_TRUE_RANDOM_ENTROPY_SOURCE, "EZSP_GET_TRUE_RANDOM_ENTROPY_SOURCE" },
		/* Networking Frames */
		{ EZSP_SET_MANUFACTURER_CODE, "EZSP_SET_MANUFACTURER_CODE" },
		{ EZSP_SET_POWER_DESCRIPTOR, "EZSP_SET_POWER_DESCRIPTOR" },
		{ EZSP_NETWORK_INIT, "EZSP_NETWORK_INIT" },
		{ EZSP_NETWORK_STATE, "EZSP_NETWORK_STATE" },
		{ EZSP_STACK_STATUS_HANDLER, "EZSP_STACK_STATUS_HANDLER" },
		{ EZSP_START_SCAN, "EZSP_START_SCAN" },
		{ EZSP_ENERGY_SCAN_RESULT_HANDLER, "EZSP_ENERGY_SCAN_RESULT_HANDLER" },
		{ EZSP_NETWORK_FOUND_HANDLER, "EZSP_NETWORK_FOUND_HANDLER" },
		{ EZSP_SCAN_COMPLETE_HANDLER, "EZSP_SCAN_COMPLETE_HANDLER" },
		{ EZSP_UNUSED_PAN_ID_FOUND_HANDLER, "EZSP_UNUSED_PAN_ID_FOUND_HANDLER" },
		{ EZSP_FIND_UNUSED_PAN_ID, "EZSP_FIND_UNUSED_PAN_ID" },
		{ EZSP_STOP_SCAN, "EZSP_STOP_SCAN" },
		{ EZSP_FORM_NETWORK, "EZSP_FORM_NETWORK" },
		{ EZSP_JOIN_NETWORK, "EZSP_JOIN_NETWORK" },
		{ EZSP_LEAVE_NETWORK, "EZSP_LEAVE_NETWORK" },
		{ EZSP_FIND_AND_REJOIN_NETWORK, "EZSP_FIND_AND_REJOIN_NETWORK" },
		{ EZSP_PERMIT_JOINING, "EZSP_PERMIT_JOINING" },
		{ EZSP_CHILD_JOIN_HANDLER, "EZSP_CHILD_JOIN_HANDLER" },
		{ EZSP_ENERGY_SCAN_REQUEST, "EZSP_ENERGY_SCAN_REQUEST" },
		{ EZSP_GET_NETWORK_PARAMETERS, "EZSP_GET_NETWORK_PARAMETERS" },
		{ EZSP_GET_RADIO_PARAMETERS, "EZSP_GET_RADIO_PARAMETERS" },
		{ EZSP_GET_PARENT_CHILD_PARAMETERS, "EZSP_GET_PARENT_CHILD_PARAMETERS" },
		{ EZSP_GET_CHILD_DATA, "EZSP_GET_CHILD_DATA" },
		{ EZSP_GET_SOURCE_ROUTE_TABLE_TOTAL_SIZE, "EZSP_GET_SOURCE_ROUTE_TABLE_TOTAL_SIZE" },
		{ EZSP_GET_SOURCE_ROUTE_TABLE_FILLED_SIZE, "EZSP_GET_SOURCE_ROUTE_TABLE_FILLED_SIZE" },
		{ EZSP_GET_SOURCE_ROUTE_TABLE_ENTRY, "EZSP_GET_SOURCE_ROUTE_TABLE_ENTRY" },
		{ EZSP_GET_NEIGHBOR, "EZSP_GET_NEIGHBOR" },
		{ EZSP_SET_ROUTING_SHORTCUT_THRESHOLD, "EZSP_SET_ROUTING_SHORTCUT_THRESHOLD" },
		{ EZSP_GET_ROUTING_SHORTCUT_THRESHOLD, "EZSP_GET_ROUTING_SHORTCUT_THRESHOLD" },
		{ EZSP_NEIGHBOR_COUNT, "EZSP_NEIGHBOR_COUNT" },
		{ EZSP_GET_ROUTE_TABLE_ENTRY, "EZSP_GET_ROUTE_TABLE_ENTRY" },
		{ EZSP_SET_RADIO_POWER, "EZSP_SET_RADIO_POWER" },
		{ EZSP_SET_RADIO_CHANNEL, "EZSP_SET_RADIO_CHANNEL" },
		{ EZSP_SET_CONCENTRATOR, "EZSP_SET_CONCENTRATOR" },
		{ EZSP_SET_BROKEN_ROUTE_ERROR_CODE, "EZSP_SET_BROKEN_ROUTE_ERROR_CODE" },
		{ EZSP_MULTI_PHY_START, "EZSP_MULTI_PHY_START" },
		{ EZSP_MULTI_PHY_STOP, "EZSP_MULTI_PHY_STOP" },
		{ EZSP_MULTI_PHY_SET_RADIO_POWER, "EZSP_MULTI_PHY_SET_RADIO_POWER" },
		{ EZSP_SEND_LINK_POWER_DELTA_REQUEST, "EZSP_SEND_LINK_POWER_DELTA_REQUEST" },
		{ EZSP_MULTI_PHY_SET_RADIO_CHANNEL, "EZSP_MULTI_PHY_SET_RADIO_CHANNEL" },
		{ EZSP_GET_DUTY_CYCLE_STATE, "EZSP_GET_DUTY_CYCLE_STATE" },
		{ EZSP_SET_DUTY_CYCLE_LIMITS_IN_STACK, "EZSP_SET_DUTY_CYCLE_LIMITS_IN_STACK" },
		{ EZSP_GET_DUTY_CYCLE_LIMITS, "EZSP_GET_DUTY_CYCLE_LIMITS" },
		{ EZSP_GET_CURRENT_DUTY_CYCLE, "EZSP_GET_CURRENT_DUTY_CYCLE" },
		{ EZSP_DUTY_CYCLE_HANDLER, "EZSP_DUTY_CYCLE_HANDLER" },
		/* Binding Frames */
		{ EZSP_CLEAR_BINDING_TABLE, "EZSP_CLEAR_BINDING_TABLE" },
		{ EZSP_SET_BINDING, "EZSP_SET_BINDING" },
		{ EZSP_GET_BINDING, "EZSP_GET_BINDING" },
		{ EZSP_DELETE_BINDING, "EZSP_DELETE_BINDING" },
		{ EZSP_BINDING_IS_ACTIVE, "EZSP_BINDING_IS_ACTIVE" },
		{ EZSP_GET_BINDING_REMOTE_NODE_ID, "EZSP_GET_BINDING_REMOTE_NODE_ID" },
		{ EZSP_SET_BINDING_REMOTE_NODE_ID, "EZSP_SET_BINDING_REMOTE_NODE_ID" },
		{ EZSP_REMOTE_SET_BINDING_HANDLER, "EZSP_REMOTE_SET_BINDING_HANDLER" },
		{ EZSP_REMOTE_DELETE_BINDING_HANDLER, "EZSP_REMOTE_DELETE_BINDING_HANDLER" },
		/* Messaging Frames */
		{ EZSP_MAXIMUM_PAYLOAD_LENGTH, "EZSP_MAXIMUM_PAYLOAD_LENGTH" },
		{ EZSP_SEND_UNICAST, "EZSP_SEND_UNICAST" },
		{ EZSP_SEND_BROADCAST, "EZSP_SEND_BROADCAST" },
		{ EZSP_PROXY_BROADCAST, "EZSP_PROXY_BROADCAST" },
		{ EZSP_SEND_MULTICAST, "EZSP_SEND_MULTICAST" },
		{ EZSP_SEND_MULTICAST_WITH_ALIAS, "EZSP_SEND_MULTICAST_WITH_ALIAS" },
		{ EZSP_SEND_REPLY, "EZSP_SEND_REPLY" },
		{ EZSP_MESSAGE_SENT_HANDLER, "EZSP_MESSAGE_SENT_HANDLER" },
		{ EZSP_SEND_MANY_TO_ONE_ROUTE_REQUEST, "EZSP_SEND_MANY_TO_ONE_ROUTE_REQUEST" },
		{ EZSP_POLL_FOR_DATA, "EZSP_POLL_FOR_DATA" },
		{ EZSP_POLL_COMPLETE_HANDLER, "EZSP_POLL_COMPLETE_HANDLER" },
		{ EZSP_POLL_HANDLER, "EZSP_POLL_HANDLER" },
		{ EZSP_INCOMING_SENDER_EUI64_HANDLER, "EZSP_INCOMING_SENDER_EUI64_HANDLER" },
		{ EZSP_INCOMING_MESSAGE_HANDLER, "EZSP_INCOMING_MESSAGE_HANDLER" },
		{ EZSP_INCOMING_ROUTE_RECORD_HANDLER, "EZSP_INCOMING_ROUTE_RECORD_HANDLER" },
		{ EZSP_CHANGE_SOURCE_ROUTE_HANDLER, "EZSP_CHANGE_SOURCE_ROUTE_HANDLER" },
		{ EZSP_SET_SOURCE_ROUTE, "EZSP_SET_SOURCE_ROUTE" },
		{ EZSP_INCOMING_MANY_TO_ONE_ROUTE_REQUEST_HANDLER, "EZSP_INCOMING_MANY_TO_ONE_ROUTE_REQUEST_HANDLER" },
		{ EZSP_INCOMING_ROUTE_ERROR_HANDLER, "EZSP_INCOMING_ROUTE_ERROR_HANDLER" },
		{ EZSP_ADDRESS_TABLE_ENTRY_IS_ACTIVE, "EZSP_ADDRESS_TABLE_ENTRY_IS_ACTIVE" },
		{ EZSP_SET_ADDRESS_TABLE_REMOTE_EUI64, "EZSP_SET_ADDRESS_TABLE_REMOTE_EUI64" },
		{ EZSP_SET_ADDRESS_TABLE_REMOTE_NODE_ID, "EZSP_SET_ADDRESS_TABLE_REMOTE_NODE_ID" },
		{ EZSP_GET_ADDRESS_TABLE_REMOTE_EUI64, "EZSP_GET_ADDRESS_TABLE_REMOTE_EUI64" },
		{ EZSP_GET_ADDRESS_TABLE_REMOTE_NODE_ID, "EZSP_GET_ADDRESS_TABLE_REMOTE_NODE_ID" },
		{ EZSP_SET_EXTENDED_TIMEOUT, "EZSP_SET_EXTENDED_TIMEOUT" },
		{ EZSP_GET_EXTENDED_TIMEOUT, "EZSP_GET_EXTENDED_TIMEOUT" },
		{ EZSP_REPLACE_ADDRESS_TABLE_ENTRY, "EZSP_REPLACE_ADDRESS_TABLE_ENTRY" },
		{ EZSP_LOOKUP_NODE_ID_BY_EUI64, "EZSP_LOOKUP_NODE_ID_BY_EUI64" },
		{ EZSP_LOOKUP_EUI64_BY_NODE_ID, "EZSP_LOOKUP_EUI64_BY_NODE_ID" },
		{ EZSP_GET_MULTICAST_TABLE_ENTRY, "EZSP_GET_MULTICAST_TABLE_ENTRY" },
		{ EZSP_SET_MULTICAST_TABLE_ENTRY, "EZSP_SET_MULTICAST_TABLE_ENTRY" },
		{ EZSP_ID_CONFLICT_HANDLER, "EZSP_ID_CONFLICT_HANDLER" },
		{ EZSP_WRITE_NODE_DATA, "EZSP_WRITE_NODE_DATA" },
		{ EZSP_SEND_RAW_MESSAGE, "EZSP_SEND_RAW_MESSAGE" },
		{ EZSP_MAC_PASSTHROUGH_MESSAGE_HANDLER, "EZSP_MAC_PASSTHROUGH_MESSAGE_HANDLER" },
		{ EZSP_MAC_FILTER_MATCH_MESSAGE_HANDLER, "EZSP_MAC_FILTER_MATCH_MESSAGE_HANDLER" },
		{ EZSP_RAW_TRANSMIT_COMPLETE_HANDLER, "EZSP_RAW_TRANSMIT_COMPLETE_HANDLER" },
		{ EZSP_SET_MAC_POLL_CCA_WAIT_TIME, "EZSP_SET_MAC_POLL_CCA_WAIT_TIME" },
		{ EZSP_SET_BEACON_CLASSIFICATION_PARAMS, "EZSP_SET_BEACON_CLASSIFICATION_PARAMS" },
		{ EZSP_GET_BEACON_CLASSIFICATION_PARAMS, "EZSP_GET_BEACON_CLASSIFICATION_PARAMS" },
		/* Security Frames */
		{ EZSP_SET_INITIAL_SECURITY_STATE, "EZSP_SET_INITIAL_SECURITY_STATE" },
		{ EZSP_GET_CURRENT_SECURITY_STATE, "EZSP_GET_CURRENT_SECURITY_STATE" },
		{ EZSP_GET_KEY, "EZSP_GET_KEY" },
		{ EZSP_SWITCH_NETWORK_KEY_HANDLER, "EZSP_SWITCH_NETWORK_KEY_HANDLER" },
		{ EZSP_GET_KEY_TABLE_ENTRY, "EZSP_GET_KEY_TABLE_ENTRY" },
		{ EZSP_SET_KEY_TABLE_ENTRY, "EZSP_SET_KEY_TABLE_ENTRY" },
		{ EZSP_FIND_KEY_TABLE_ENTRY, "EZSP_FIND_KEY_TABLE_ENTRY" },
		{ EZSP_ADD_OR_UPDATE_KEY_TABLE_ENTRY, "EZSP_ADD_OR_UPDATE_KEY_TABLE_ENTRY" },
		{ EZSP_SEND_TRUST_CENTER_LINK_KEY, "EZSP_SEND_TRUST_CENTER_LINK_KEY" },
		{ EZSP_ERASE_KEY_TABLE_ENTRY, "EZSP_ERASE_KEY_TABLE_ENTRY" },
		{ EZSP_CLEAR_KEY_TABLE, "EZSP_CLEAR_KEY_TABLE" },
		{ EZSP_REQUEST_LINK_KEY, "EZSP_REQUEST_LINK_KEY" },
		{ EZSP_UPDATE_TC_LINK_KEY, "EZSP_UPDATE_TC_LINK_KEY" },
		{ EZSP_ZIGBEE_KEY_ESTABLISHMENT_HANDLER, "EZSP_ZIGBEE_KEY_ESTABLISHMENT_HANDLER" },
		{ EZSP_ADD_TRANSIENT_LINK_KEY, "EZSP_ADD_TRANSIENT_LINK_KEY" },
		{ EZSP_CLEAR_TRANSIENT_LINK_KEYS, "EZSP_CLEAR_TRANSIENT_LINK_KEYS" },
		{ EZSP_GET_TRANSIENT_LINK_KEY, "EZSP_GET_TRANSIENT_LINK_KEY" },
		{ EZSP_GET_TRANSIENT_KEY_TABLE_ENTRY, "EZSP_GET_TRANSIENT_KEY_TABLE_ENTRY" },
		/* Trust Center Frames */
		{ EZSP_TRUST_CENTER_JOIN_HANDLER, "EZSP_TRUST_CENTER_JOIN_HANDLER" },
		{ EZSP_BROADCAST_NEXT_NETWORK_KEY, "EZSP_BROADCAST_NEXT_NETWORK_KEY" },
		{ EZSP_BROADCAST_NETWORK_KEY_SWITCH, "EZSP_BROADCAST_NETWORK_KEY_SWITCH" },
		{ EZSP_BECOME_TRUST_CENTER, "EZSP_BECOME_TRUST_CENTER" },
		{ EZSP_AES_MMO_HASH, "EZSP_AES_MMO_HASH" },
		{ EZSP_REMOVE_DEVICE, "EZSP_REMOVE_DEVICE" },
		{ EZSP_UNICAST_NWK_KEY_UPDATE, "EZSP_UNICAST_NWK_KEY_UPDATE" },
		/* Certificate Based Key Exchange (CBKE) */
		{ EZSP_GENERATE_CBKE_KEYS, "EZSP_GENERATE_CBKE_KEYS" },
		{ EZSP_GENERATE_CBKE_KEYS_HANDLER, "EZSP_GENERATE_CBKE_KEYS_HANDLER" },
		{ EZSP_CALCULATE_SMACS, "EZSP_CALCULATE_SMACS" },
		{ EZSP_CALCULATE_SMACS_HANDLER, "EZSP_CALCULATE_SMACS_HANDLER" },
		{ EZSP_GENERATE_CBKE_KEYS283K1, "EZSP_GENERATE_CBKE_KEYS283K1" },
		{ EZSP_GENERATE_CBKE_KEYS_HANDLER283K1, "EZSP_GENERATE_CBKE_KEYS_HANDLER283K1" },
		{ EZSP_CALCULATE_SMACS283K1, "EZSP_CALCULATE_SMACS283K1" },
		{ EZSP_CALCULATE_SMACS_HANDLER283K1, "EZSP_CALCULATE_SMACS_HANDLER283K1" },
		{ EZSP_CLEAR_TEMPORARY_DATA_MAYBE_STORE_LINK_KEY, "EZSP_CLEAR_TEMPORARY_DATA_MAYBE_STORE_LINK_KEY" },
		{ EZSP_CLEAR_TEMPORARY_DATA_MAYBE_STORE_LINK_KEY283K1, "EZSP_CLEAR_TEMPORARY_DATA_MAYBE_STORE_LINK_KEY283K1" },
		{ EZSP_GET_CERTIFICATE, "EZSP_GET_CERTIFICATE" },
		{ EZSP_GET_CERTIFICATE283K1, "EZSP_GET_CERTIFICATE283K1" },
		{ EZSP_DSA_SIGN, "EZSP_DSA_SIGN" },
		{ EZSP_DSA_SIGN_HANDLER, "EZSP_DSA_SIGN_HANDLER" },
		{ EZSP_DSA_VERIFY, "EZSP_DSA_VERIFY" },
		{ EZSP_DSA_VERIFY_HANDLER, "EZSP_DSA_VERIFY_HANDLER" },
		{ EZSP_DSA_VERIFY283K1, "EZSP_DSA_VERIFY283K1" },
		{ EZSP_SET_PREINSTALLED_CBKE_DATA, "EZSP_SET_PREINSTALLED_CBKE_DATA" },
		{ EZSP_SAVE_PREINSTALLED_CBKE_DATA283K1, "EZSP_SAVE_PREINSTALLED_CBKE_DATA283K1" },
		/* Mfglib */
		{ EZSP_MFGLIB_RX_HANDLER, "EZSP_MFGLIB_RX_HANDLER" },
		/* --- */
		/* Bootloader */
		{ EZSP_LAUNCH_STANDALONE_BOOTLOADER, "EZSP_LAUNCH_STANDALONE_BOOTLOADER" },
		{ EZSP_SEND_BOOTLOAD_MESSAGE, "EZSP_SEND_BOOTLOAD_MESSAGE" },
		{ EZSP_GET_STANDALONE_BOOTLOADER_VERSION_PLAT_MICRO_PHY, "EZSP_GET_STANDALONE_BOOTLOADER_VERSION_PLAT_MICRO_PHY" },
		{ EZSP_INCOMING_BOOTLOAD_MESSAGE_HANDLER, "EZSP_INCOMING_BOOTLOAD_MESSAGE_HANDLER" },
		{ EZSP_BOOTLOAD_TRANSMIT_COMPLETE_HANDLER, "EZSP_BOOTLOAD_TRANSMIT_COMPLETE_HANDLER" },
		{ EZSP_AES_ENCRYPT, "EZSP_AES_ENCRYPT" },
		{ EZSP_OVERRIDE_CURRENT_CHANNEL, "EZSP_OVERRIDE_CURRENT_CHANNEL" },
		/* ZLL */
		/* --- */
		/* WWAH */
		/* --- */
		/* Green Power */
		{ EZSP_GP_PROXY_TABLE_PROCESS_GP_PAIRING, "EZSP_GP_PROXY_TABLE_PROCESS_GP_PAIRING" },
		{ EZSP_D_GP_SEND, "EZSP_D_GP_SEND" },
		{ EZSP_D_GP_SENT_HANDLER, "EZSP_D_GP_SENT_HANDLER" },
		{ EZSP_GPEP_INCOMING_MESSAGE_HANDLER, "EZSP_GPEP_INCOMING_MESSAGE_HANDLER" },
		{ EZSP_GP_PROXY_TABLE_GET_ENTRY, "EZSP_GP_PROXY_TABLE_GET_ENTRY" },
		{ EZSP_GP_PROXY_TABLE_LOOKUP, "EZSP_GP_PROXY_TABLE_LOOKUP" },
		{ EZSP_GP_SINK_TABLE_GET_ENTRY, "EZSP_GP_SINK_TABLE_GET_ENTRY" },
		{ EZSP_GP_SINK_TABLE_LOOKUP, "EZSP_GP_SINK_TABLE_LOOKUP" },
		{ EZSP_GP_SINK_TABLE_SET_ENTRY, "EZSP_GP_SINK_TABLE_SET_ENTRY" },
		{ EZSP_GP_SINK_TABLE_REMOVE_ENTRY, "EZSP_GP_SINK_TABLE_REMOVE_ENTRY" },
		{ EZSP_GP_SINK_TABLE_FIND_OR_ALLOCATE_ENTRY, "EZSP_GP_SINK_TABLE_FIND_OR_ALLOCATE_ENTRY" },
		{ EZSP_GP_SINK_TABLE_CLEAR_ALL, "EZSP_GP_SINK_TABLE_CLEAR_ALL" },
		{ EZSP_GP_SINK_TABLE_INIT, "EZSP_GP_SINK_TABLE_INIT" },
		/* Secure EZSP */
		/* --- */
	};
	auto   it  = MyEnumStrings.find(in);
	return it == MyEnumStrings.end() ? "OUT_OF_RANGE" : it->second;
}

std::string CEzspEnum::EmberKeyTypeToString( EmberKeyType in ) {
	const std::map<EmberKeyType,std::string> MyEnumStrings {
		{ EMBER_TRUST_CENTER_LINK_KEY, "EMBER_TRUST_CENTER_LINK_KEY" },
		{ EMBER_CURRENT_NETWORK_KEY, "EMBER_CURRENT_NETWORK_KEY" },
		{ EMBER_NEXT_NETWORK_KEY, "EMBER_NEXT_NETWORK_KEY" },
		{ EMBER_APPLICATION_LINK_KEY, "EMBER_APPLICATION_LINK_KEY" }
	};
	auto   it  = MyEnumStrings.find(in);
	return it == MyEnumStrings.end() ? "OUT_OF_RANGE" : it->second;
}


std::string CEzspEnum::EmberIncomingMessageTypeToString( EmberIncomingMessageType in ) {
	const std::map<EmberIncomingMessageType,std::string> MyEnumStrings {
		{ EMBER_INCOMING_UNICAST, "EMBER_INCOMING_UNICAST" },
		{ EMBER_INCOMING_UNICAST_REPLY, "EMBER_INCOMING_UNICAST_REPLY" },
		{ EMBER_INCOMING_MULTICAST, "EMBER_INCOMING_MULTICAST" },
		{ EMBER_INCOMING_MULTICAST_LOOPBACK, "EMBER_INCOMING_MULTICAST_LOOPBACK" },
		{ EMBER_INCOMING_BROADCAST, "EMBER_INCOMING_BROADCAST" },
		{ EMBER_INCOMING_BROADCAST_LOOPBACK, "EMBER_INCOMING_BROADCAST_LOOPBACK" },
		{ EMBER_INCOMING_MANY_TO_ONE_ROUTE_REQUEST, "EMBER_INCOMING_MANY_TO_ONE_ROUTE_REQUEST" }
	};
	auto   it  = MyEnumStrings.find(in);
	return it == MyEnumStrings.end() ? "OUT_OF_RANGE" : it->second;
}
