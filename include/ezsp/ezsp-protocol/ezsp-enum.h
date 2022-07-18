/**
 * @file ezsp-enum.h
 *
 * @brief Various EZSP-constants from the EZSP specs and related manipulation functions
 */

#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <string>

namespace NSEZSP {

typedef enum {
	E_IN_MSG_TYPE_UNICAST,
	E_IN_MSG_TYPE_UNICAST_REPLY,
	E_IN_MSG_TYPE_MULTICAST,
	E_IN_MSG_TYPE_MULTICAST_LOOPBACK,
	E_IN_MSG_TYPE_BROADCAST,
	E_IN_MSG_TYPE_BROADCAST_LOOPBACK
} EInMsgType;

typedef struct {
	EInMsgType  msg_type;
	uint16_t     sender_id;
	uint8_t      last_lqi;
	int8_t       last_rssi;
} TInMsgInfo;

typedef struct {
	std::vector<uint8_t> ext_pan_id; //  std::array<uint8_t, > ext_pan_id;
	uint16_t pan_id;
	uint8_t channel;
	bool open;
	uint8_t stack_profile;
} TNwkInfo;

typedef enum {
// Configuration Frames
	EZSP_VERSION                                  = 0x00,
	EZSP_GET_CONFIGURATION_VALUE                  = 0x52,
	EZSP_SET_CONFIGURATION_VALUE                  = 0x53,
	EZSP_ADD_ENDPOINT                             = 0x02,
	EZSP_SET_POLICY                               = 0x55,
	EZSP_GET_POLICY                               = 0x56,
	EZSP_SEND_PAN_ID_UPDATE                       = 0x57,
	EZSP_GET_VALUE                                = 0xAA,
	EZSP_GET_EXTENDED_VALUE                       = 0x03,
	EZSP_SET_VALUE                                = 0xAB,
	EZSP_SET_GPIO_CURRENT_CONFIGURATION           = 0xAC,
	EZSP_SET_GPIO_POWER_UP_DOWN_CONFIGURATION     = 0xAD,
	EZSP_SET_GPIO_RADIO_POWER_MASK                = 0xAE,
	EZSP_SET_CTUNE                                = 0xF5,
	EZSP_GET_CTUNE                                = 0xF6,

// Utilities Frames
	EZSP_NOP                                      = 0x05,
	EZSP_ECHO                                     = 0x81,
	EZSP_INVALID_COMMAND                          = 0x58,
	EZSP_CALLBACK                                 = 0x06,
	EZSP_NO_CALLBACKS                             = 0x07,
	EZSP_SET_TOKEN                                = 0x09,
	EZSP_GET_TOKEN                                = 0x0A,
	EZSP_GET_MFG_TOKEN                            = 0x0B,
	EZSP_SET_MFG_TOKEN                            = 0x0C,
	EZSP_STACK_TOKEN_CHANGED_HANDLER              = 0x0D,
	EZSP_GET_RANDOM_NUMBER                        = 0x49,
	EZSP_SET_TIMER                                = 0x0E,
	EZSP_GET_TIMER                                = 0x4E,
	EZSP_TIMER_HANDLER                            = 0x0F,
	EZSP_DEBUG_WRITE                              = 0x12,
	EZSP_READ_AND_CLEAR_COUNTERS                  = 0x65,
	EZSP_READ_COUNTERS                            = 0xF1,
	EZSP_COUNTER_ROLLOVER_HANDLER                 = 0xF2,
	EZSP_DELAY_TEST                               = 0x9D,
	EZSP_GET_LIBRARY_STATUS                       = 0x01,
	EZSP_GET_XNCP_INFO                            = 0x13,
	EZSP_CUSTOM_FRAME                             = 0x47,
	EZSP_CUSTOM_FRAME_HANDLER                     = 0x54,
	EZSP_GET_EUI64                                = 0x26,
	EZSP_GET_NODE_ID                              = 0x27,
	EZSP_GET_PHY_INTERFACE_COUNT                  = 0xFC,
	EZSP_GET_TRUE_RANDOM_ENTROPY_SOURCE           = 0x4F,

// Networking Frames
	EZSP_SET_MANUFACTURER_CODE                    = 0x15,
	EZSP_SET_POWER_DESCRIPTOR                     = 0x16,
	EZSP_NETWORK_INIT                             = 0x17,
	EZSP_NETWORK_STATE                            = 0x18,
	EZSP_STACK_STATUS_HANDLER                     = 0x19,
	EZSP_START_SCAN                               = 0x1A,
	EZSP_ENERGY_SCAN_RESULT_HANDLER               = 0x48,
	EZSP_NETWORK_FOUND_HANDLER                    = 0x1B,
	EZSP_SCAN_COMPLETE_HANDLER                    = 0x1C,
	EZSP_UNUSED_PAN_ID_FOUND_HANDLER              = 0xD2,
	EZSP_FIND_UNUSED_PAN_ID                       = 0xD3,
	EZSP_STOP_SCAN                                = 0x1D,
	EZSP_FORM_NETWORK                             = 0x1E,
	EZSP_JOIN_NETWORK                             = 0x1F,
	EZSP_LEAVE_NETWORK                            = 0x20,
	EZSP_FIND_AND_REJOIN_NETWORK                  = 0x21,
	EZSP_PERMIT_JOINING                           = 0x22,
	EZSP_CHILD_JOIN_HANDLER                       = 0x23,
	EZSP_ENERGY_SCAN_REQUEST                      = 0x9C,
	EZSP_GET_NETWORK_PARAMETERS                   = 0x28,
	EZSP_GET_RADIO_PARAMETERS                     = 0xFD,
	EZSP_GET_PARENT_CHILD_PARAMETERS              = 0x29,
	EZSP_GET_CHILD_DATA                           = 0x4A,
	EZSP_GET_SOURCE_ROUTE_TABLE_TOTAL_SIZE        = 0xC3,
	EZSP_GET_SOURCE_ROUTE_TABLE_FILLED_SIZE       = 0xC2,
	EZSP_GET_SOURCE_ROUTE_TABLE_ENTRY             = 0xC1,
	EZSP_GET_NEIGHBOR                             = 0x79,
	EZSP_SET_ROUTING_SHORTCUT_THRESHOLD           = 0xD0,
	EZSP_GET_ROUTING_SHORTCUT_THRESHOLD           = 0xD1,
	EZSP_NEIGHBOR_COUNT                           = 0x7A,
	EZSP_GET_ROUTE_TABLE_ENTRY                    = 0x7B,
	EZSP_SET_RADIO_POWER                          = 0x99,
	EZSP_SET_RADIO_CHANNEL                        = 0x9A,
	EZSP_SET_CONCENTRATOR                         = 0x10,
	EZSP_SET_BROKEN_ROUTE_ERROR_CODE              = 0x11,
	EZSP_MULTI_PHY_START                          = 0xF8,
	EZSP_MULTI_PHY_STOP                           = 0xF9,
	EZSP_MULTI_PHY_SET_RADIO_POWER                = 0xFA,
	EZSP_SEND_LINK_POWER_DELTA_REQUEST            = 0xF7,
	EZSP_MULTI_PHY_SET_RADIO_CHANNEL              = 0xFB,
	EZSP_GET_DUTY_CYCLE_STATE                     = 0x35,
	EZSP_SET_DUTY_CYCLE_LIMITS_IN_STACK           = 0x40,
	EZSP_GET_DUTY_CYCLE_LIMITS                    = 0x4B,
	EZSP_GET_CURRENT_DUTY_CYCLE                   = 0x4C,
	EZSP_DUTY_CYCLE_HANDLER                       = 0x4D,

// Binding Frames
	EZSP_CLEAR_BINDING_TABLE                      = 0x2A,
	EZSP_SET_BINDING                              = 0x2B,
	EZSP_GET_BINDING                              = 0x2C,
	EZSP_DELETE_BINDING                           = 0x2D,
	EZSP_BINDING_IS_ACTIVE                        = 0x2E,
	EZSP_GET_BINDING_REMOTE_NODE_ID               = 0x2F,
	EZSP_SET_BINDING_REMOTE_NODE_ID               = 0x30,
	EZSP_REMOTE_SET_BINDING_HANDLER               = 0x31,
	EZSP_REMOTE_DELETE_BINDING_HANDLER            = 0x32,

// Messaging Frames
	EZSP_MAXIMUM_PAYLOAD_LENGTH                   = 0x33,
	EZSP_SEND_UNICAST                             = 0x34,
	EZSP_SEND_BROADCAST                           = 0x36,
	EZSP_PROXY_BROADCAST                          = 0x37,
	EZSP_SEND_MULTICAST                           = 0x38,
	EZSP_SEND_MULTICAST_WITH_ALIAS                = 0x3A,
	EZSP_SEND_REPLY                               = 0x39,
	EZSP_MESSAGE_SENT_HANDLER                     = 0x3F,
	EZSP_SEND_MANY_TO_ONE_ROUTE_REQUEST           = 0x41,
	EZSP_POLL_FOR_DATA                            = 0x42,
	EZSP_POLL_COMPLETE_HANDLER                    = 0x43,
	EZSP_POLL_HANDLER                             = 0x44,
	EZSP_INCOMING_SENDER_EUI64_HANDLER            = 0x62,
	EZSP_INCOMING_MESSAGE_HANDLER                 = 0x45,
	EZSP_INCOMING_ROUTE_RECORD_HANDLER            = 0x59,
	EZSP_CHANGE_SOURCE_ROUTE_HANDLER              = 0xC4,
	EZSP_SET_SOURCE_ROUTE                         = 0x5A,
	EZSP_INCOMING_MANY_TO_ONE_ROUTE_REQUEST_HANDLER = 0x7D,
	EZSP_INCOMING_ROUTE_ERROR_HANDLER             = 0x80,
	EZSP_ADDRESS_TABLE_ENTRY_IS_ACTIVE            = 0x5B,
	EZSP_SET_ADDRESS_TABLE_REMOTE_EUI64           = 0x5C,
	EZSP_SET_ADDRESS_TABLE_REMOTE_NODE_ID         = 0x5D,
	EZSP_GET_ADDRESS_TABLE_REMOTE_EUI64           = 0x5E,
	EZSP_GET_ADDRESS_TABLE_REMOTE_NODE_ID         = 0x5F,
	EZSP_SET_EXTENDED_TIMEOUT                     = 0x7E,
	EZSP_GET_EXTENDED_TIMEOUT                     = 0x7F,
	EZSP_REPLACE_ADDRESS_TABLE_ENTRY              = 0x82,
	EZSP_LOOKUP_NODE_ID_BY_EUI64                  = 0x60,
	EZSP_LOOKUP_EUI64_BY_NODE_ID                  = 0x61,
	EZSP_GET_MULTICAST_TABLE_ENTRY                = 0x63,
	EZSP_SET_MULTICAST_TABLE_ENTRY                = 0x64,
	EZSP_ID_CONFLICT_HANDLER                      = 0x7C,
	EZSP_WRITE_NODE_DATA                          = 0xFE,
	EZSP_SEND_RAW_MESSAGE                         = 0x96,
	EZSP_MAC_PASSTHROUGH_MESSAGE_HANDLER          = 0x97,
	EZSP_MAC_FILTER_MATCH_MESSAGE_HANDLER         = 0x46,
	EZSP_RAW_TRANSMIT_COMPLETE_HANDLER            = 0x98,
	EZSP_SET_MAC_POLL_CCA_WAIT_TIME               = 0xF4,
	EZSP_SET_BEACON_CLASSIFICATION_PARAMS         = 0xEF,
	EZSP_GET_BEACON_CLASSIFICATION_PARAMS         = 0xF3,

// Security Frames
	EZSP_SET_INITIAL_SECURITY_STATE               = 0x68,
	EZSP_GET_CURRENT_SECURITY_STATE               = 0x69,
	EZSP_GET_KEY                                  = 0x6a,
	EZSP_SWITCH_NETWORK_KEY_HANDLER               = 0x6e,
	EZSP_GET_KEY_TABLE_ENTRY                      = 0x71,
	EZSP_SET_KEY_TABLE_ENTRY                      = 0x72,
	EZSP_FIND_KEY_TABLE_ENTRY                     = 0x75,
	EZSP_ADD_OR_UPDATE_KEY_TABLE_ENTRY            = 0x66,
	EZSP_SEND_TRUST_CENTER_LINK_KEY               = 0x67,
	EZSP_ERASE_KEY_TABLE_ENTRY                    = 0x76,
	EZSP_CLEAR_KEY_TABLE                          = 0xB1,
	EZSP_REQUEST_LINK_KEY                         = 0x14,
	EZSP_UPDATE_TC_LINK_KEY                       = 0x6C,
	EZSP_ZIGBEE_KEY_ESTABLISHMENT_HANDLER         = 0x9B,
	EZSP_ADD_TRANSIENT_LINK_KEY                   = 0xAF,
	EZSP_CLEAR_TRANSIENT_LINK_KEYS                = 0x6B,
	EZSP_GET_TRANSIENT_LINK_KEY                   = 0xCE,
	EZSP_GET_TRANSIENT_KEY_TABLE_ENTRY            = 0x6D,

// Trust Center Frames
	EZSP_TRUST_CENTER_JOIN_HANDLER                = 0x24,
	EZSP_BROADCAST_NEXT_NETWORK_KEY               = 0x73,
	EZSP_BROADCAST_NETWORK_KEY_SWITCH             = 0x74,
	EZSP_BECOME_TRUST_CENTER                      = 0x77,
	EZSP_AES_MMO_HASH                             = 0x6F,
	EZSP_REMOVE_DEVICE                            = 0xA8,
	EZSP_UNICAST_NWK_KEY_UPDATE                   = 0xA9,

// Certificate Based Key Exchange (CBKE)
	EZSP_GENERATE_CBKE_KEYS                       = 0xA4,
	EZSP_GENERATE_CBKE_KEYS_HANDLER               = 0x9E,
	EZSP_CALCULATE_SMACS                          = 0x9F,
	EZSP_CALCULATE_SMACS_HANDLER                  = 0xA0,
	EZSP_GENERATE_CBKE_KEYS283K1                  = 0xE8,
	EZSP_GENERATE_CBKE_KEYS_HANDLER283K1          = 0xE9,
	EZSP_CALCULATE_SMACS283K1                     = 0xEA,
	EZSP_CALCULATE_SMACS_HANDLER283K1             = 0xEB,
	EZSP_CLEAR_TEMPORARY_DATA_MAYBE_STORE_LINK_KEY = 0xA1,
	EZSP_CLEAR_TEMPORARY_DATA_MAYBE_STORE_LINK_KEY283K1 = 0xEE,
	EZSP_GET_CERTIFICATE                          = 0xA5,
	EZSP_GET_CERTIFICATE283K1                     = 0xEC,
	EZSP_DSA_SIGN                                 = 0xA6,
	EZSP_DSA_SIGN_HANDLER                         = 0xA7,
	EZSP_DSA_VERIFY                               = 0xA3,
	EZSP_DSA_VERIFY_HANDLER                       = 0x78,
	EZSP_DSA_VERIFY283K1                          = 0xB0,
	EZSP_SET_PREINSTALLED_CBKE_DATA               = 0xA2,
	EZSP_SAVE_PREINSTALLED_CBKE_DATA283K1         = 0xED,

// Mfglib
	EZSP_MFGLIB_START                             = 0x83,
	EZSP_MFGLIB_END                               = 0x84,
	EZSP_MFGLIB_START_TONE                        = 0x85,
	EZSP_MFGLIB_STOP_TONE                         = 0x86,
	EZSP_MFGLIB_START_STREAM                      = 0x87,
	EZSP_MFGLIB_STOP_STREAM                       = 0x88,
	EZSP_MFGLIB_SEND_PACKET                       = 0x89,
	EZSP_MFGLIB_SET_CHANNEL                       = 0x8a,
	EZSP_MFGLIB_GET_CHANNEL                       = 0x8b,
	EZSP_MFGLIB_SET_POWER                         = 0x8c,
	EZSP_MFGLIB_GET_POWER                         = 0x8d,
	EZSP_MFGLIB_RX_HANDLER                        = 0x8e,

// Bootloader
	EZSP_LAUNCH_STANDALONE_BOOTLOADER             = 0x8f,
	EZSP_SEND_BOOTLOAD_MESSAGE                    = 0x90,
	EZSP_GET_STANDALONE_BOOTLOADER_VERSION_PLAT_MICRO_PHY = 0x91,
	EZSP_INCOMING_BOOTLOAD_MESSAGE_HANDLER        = 0x92,
	EZSP_BOOTLOAD_TRANSMIT_COMPLETE_HANDLER       = 0x93,
	EZSP_AES_ENCRYPT                              = 0x94,
	EZSP_OVERRIDE_CURRENT_CHANNEL                 = 0x95,

// ZLL
	EZSP_ZLL_NETWORK_OPS                          = 0xB2,
	EZSP_ZLL_SET_INITIAL_SECURITY_STATE           = 0xB3,
	EZSP_ZLL_SET_SECURITY_STATE_WITHOUT_KEY       = 0xCF,
	EZSP_ZLL_START_SCAN                           = 0xB4,
	EZSP_ZLL_SET_RX_ON_WHEN_IDLE                  = 0xB5,
	EZSP_ZLL_NETWORK_FOUND_HANDLER                = 0xB6,
	EZSP_ZLL_SCAN_COMPLETE_HANDLER                = 0xB7,
	EZSP_ZLL_ADDRESS_ASSIGNMENT_HANDLER           = 0xB8,
	EZSP_SET_LOGICAL_AND_RADIO_CHANNEL            = 0xB9,
	EZSP_GET_LOGICAL_CHANNEL                      = 0xBA,
	EZSP_ZLL_TOUCH_LINK_TARGET_HANDLER            = 0xBB,
	EZSP_ZLL_GET_TOKENS                           = 0xBC,
	EZSP_ZLL_SET_DATA_TOKEN                       = 0xBD,
	EZSP_ZLL_SET_NON_ZLL_NETWORK                  = 0xBF,
	EZSP_IS_ZLL_NETWORK                           = 0xBE,
	EZSP_ZLL_SET_RADIO_IDLE_MODE                  = 0xD4,
	EZSP_SET_ZLL_NODE_TYPE                        = 0xD5,
	EZSP_SET_ZLL_ADDITIONAL_STATE                 = 0xD6,
	EZSP_ZLL_OPERATION_IN_PROGRESS                = 0xD7,
	EZSP_ZLL_RX_ON_WHEN_IDLE_GET_ACTIVE           = 0xD8,
	EZSP_GET_ZLL_PRIMARY_CHANNEL_MASK             = 0xD9,
	EZSP_GET_ZLL_SECONDARY_CHANNEL_MASK           = 0xDA,
	EZSP_SET_ZLL_PRIMARY_CHANNEL_MASK             = 0xDB,
	EZSP_SET_ZLL_SECONDARY_CHANNEL_MASK           = 0xDC,
	EZSP_ZLL_CLEAR_TOKENS                         = 0x25,

// WWAH
	EZSP_SET_PARENT_CLASSIFICATION_ENABLED        = 0xE7,
	EZSP_GET_PARENT_CLASSIFICATION_ENABLED        = 0xF0,
	EZSP_SET_LONG_UP_TIME                         = 0xE3,
	EZSP_SET_HUB_CONNECTIVITY                     = 0xE4,
	EZSP_IS_UP_TIME_LONG                          = 0xE5,
	EZSP_IS_HUB_CONNECTED                         = 0xE6,

// Green Power
	EZSP_GP_PROXY_TABLE_PROCESS_GP_PAIRING        = 0xC9,
	EZSP_D_GP_SEND                                = 0xC6,
	EZSP_D_GP_SENT_HANDLER                        = 0xC7,
	EZSP_GPEP_INCOMING_MESSAGE_HANDLER            = 0xC5,
	EZSP_GP_PROXY_TABLE_GET_ENTRY                 = 0xC8,
	EZSP_GP_PROXY_TABLE_LOOKUP                    = 0xC0,
	EZSP_GP_SINK_TABLE_GET_ENTRY                  = 0xDD,
	EZSP_GP_SINK_TABLE_LOOKUP                     = 0xDE,
	EZSP_GP_SINK_TABLE_SET_ENTRY                  = 0xDF,
	EZSP_GP_SINK_TABLE_REMOVE_ENTRY               = 0xE0,
	EZSP_GP_SINK_TABLE_FIND_OR_ALLOCATE_ENTRY     = 0xE1,
	EZSP_GP_SINK_TABLE_CLEAR_ALL                  = 0xE2,
	EZSP_GP_SINK_TABLE_INIT                       = 0x70,

// Secure EZSP
	EZSP_SET_SECURITY_KEY                         = 0xCA,
	EZSP_SET_SECURITY_PARAMETERS                  = 0xCB,
	EZSP_RESET_TO_FACTORY_DEFAULTS                = 0xCC,
	EZSP_GET_SECURITY_KEY_STATUS                  = 0xCD,

	// patch pour la version 1.8, retire les retry lors de l'envoi du broadcast
	// EZSP_SEND_BROADCAST_PATCH                     = 0x40U
} EEzspCmd;

typedef enum {
	EMBER_SUCCESS = 0x00U,
	EMBER_ERR_FATAL = 0x01U,
	EMBER_NO_BUFFERS = 0x18U,

	// the poll is success but there is no pending data exists for device doing
	// a data poll.
	EMBER_MAC_NO_DATA = 0x31U,

	// emberStartScan was called with an invalid channel mask.
	EMBER_MAC_INVALID_CHANNEL_MASK  = 0x35U,

	// The MAC can't complete this task because it is scanning
	EMBER_MAC_SCANNING = 0x3DU,

	// Poll_complete_handler status, when the parent of a zed doesn't answer
	EMBER_MAC_NO_ACK_RECEIVED = 0x40U,

	// he APS layer attempted to send or deliver a message, but it failed
	EMBER_DELIVERY_FAILED = 0x66U,

	// The API call is not allowed given the current state of the stack
	EMBER_INVALID_CALL = 0x70U,

	// The maximum number of in-flight messages (i.e.
	// EMBER_APS_UNICAST_MESSAGE_COUNT) has been reached.
	EMBER_MAX_MESSAGE_LIMIT_REACHED = 0x72U,

	EMBER_ADDRESS_TABLE_ENTRY_IS_ACTIVE = 0x76U,

	// Lionel: the 2 values below are undocumented but have been guessed from GP behaviour
	// Use with caution (only for debug) as these values may change with time
	// Source ID and key is known by sink, but a GP frame was receive with a MIC than does not match the known key
	UNDOCUMENTED_WRONG_MIC_FOR_SOURCE_ID = 0x7EU,

	// No key is known for this source ID, so authentication could not be checked for a received GP frame
	UNDOCUMENTED_UNKNOWN_KEY_FOR_SOURCE_ID = 0x7FU,

	// The stack software has completed initialization and is ready
	// to send and receive packets over the air
	EMBER_NETWORK_UP = 0x90U,
	// The network is not operating
	EMBER_NETWORK_DOWN = 0x91U,
	// The node has not joined a network.
	EMBER_NOT_JOINED = 0x93U,
	// An attempt to join a network failed
	EMBER_JOIN_FAILED = 0x94U,
	// After moving, a mobile node's attempt to re-establish contact with the
	// network failed
	EMBER_MOVE_FAILED = 0x96U,
	// The network has been opened for joining.
	EMBER_NETWORK_OPENED = 0x9CU,
	// The network has been closed for joining.
	EMBER_NETWORK_CLOSED = 0x9DU,
	// A message cannot be sent because the network is currently overloaded
	EMBER_NETWORK_BUSY = 0xA1U,

	// There was an attempt to form or join a network with security without calling emberSetInitialSecurityState() first
	EMBER_SECURITY_STATE_NOT_SET = 0xA8U,

	// An attempt to join or rejoin the network failed because
	// no router beacons could be heard by the joining node.
	EMBER_NO_BEACONS = 0xABU,

	// An attempt was made to join a Secured Network using a pre-configured
	// key, but the Trust Center sent back a Network Key in-the-clear when
	// an encrypted Network Key was required. (::EMBER_REQUIRE_ENCRYPTED_KEY).
	EMBER_RECEIVED_KEY_IN_THE_CLEAR =  0xACU,

	// An attempt was made to join a Secured Network, but the device did
	// not receive a Network Key.
	EMBER_NO_NETWORK_KEY_RECEIVED = 0xADU
} EEmberStatus;

typedef enum {
	/**
	 * The number of packet buffers available to the stack. When set to the special value 0xFF, the
	 * NCP will allocate all remaining configuration RAM towards packet buffers, such that the
	 * resulting count will be the largest whole number of packet buffers that can fit into the
	 * available memory.
	 */
	EZSP_CONFIG_PACKET_BUFFER_COUNT = 0x0001,

	/**
	 * The maximum number of router neighbors the stack can
	 * keep track of. A neighbor is a node within radio range.
	 */
	EZSP_CONFIG_NEIGHBOR_TABLE_SIZE = 0x0002,

	/**
	 * The maximum number of APS retried messages the
	 * stack can be transmitting at any time.
	 */
	EZSP_CONFIG_APS_UNICAST_MESSAGE_COUNT = 0x0003,

	/**
	 * The maximum number of non-volatile bindings supported by the stack.
	 */
	EZSP_CONFIG_BINDING_TABLE_SIZE = 0x0004,

	/**
	 * The maximum number of EUI64 to network address associations that the stack can maintain for
	 * the application. (Note, the total number of such address associations maintained by the NCP
	 * is the sum of the value of this setting and the value of
	 * ::EZSP_CONFIG_TRUST_CENTER_ADDRESS_CACHE_SIZE.).
	 */
	EZSP_CONFIG_ADDRESS_TABLE_SIZE = 0x0005,

	/**
	 * The maximum number of multicast groups that the
	 * device may be a member of.
	 */
	EZSP_CONFIG_MULTICAST_TABLE_SIZE = 0x0006,

	/**
	 * The maximum number of destinations to which a node
	 * can route messages. This includes both messages
	 * originating at this node and those relayed for others.
	 */
	EZSP_CONFIG_ROUTE_TABLE_SIZE = 0x0007,

	/**
	 * The number of simultaneous route discoveries that a
	 * node will support.
	 */
	EZSP_CONFIG_DISCOVERY_TABLE_SIZE = 0x0008,


	/**
	 * Specifies the stack profile.
	 */
	EZSP_CONFIG_STACK_PROFILE = 0x000C,

	/**
	 * The security level used for security at the MAC and network layers. The supported values are 0
	 * (no security) and 5 (payload is encrypted and a four-byte MIC is used for authentication).
	 */
	EZSP_CONFIG_SECURITY_LEVEL = 0x000D,

	/**
	 * The maximum number of hops for a message.
	 */
	EZSP_CONFIG_MAX_HOPS = 0x0010,

	/**
	 * The maximum number of end device children that a router will support.
	 */
	EZSP_CONFIG_MAX_END_DEVICE_CHILDREN = 0x0011,

	/**
	 * The maximum amount of time that the MAC will hold a message for indirect transmission to a
	 * child.
	 */
	EZSP_CONFIG_INDIRECT_TRANSMISSION_TIMEOUT = 0x0012,

	/**
	 * The maximum amount of time that an end device child
	 * can wait between polls. If no poll is heard within this
	 * timeout, then the parent removes the end device from
	 * its tables.
	 */
	EZSP_CONFIG_END_DEVICE_POLL_TIMEOUT = 0x0013,

	/**
	 * The maximum amount of time that a mobile node can
	 * wait between polls. If no poll is heard within this timeout,
	 * then the parent removes the mobile node from its
	 * tables.
	 */
	EZSP_CONFIG_MOBILE_NODE_POLL_TIMEOUT = 0x0014,

	/**
	 * The number of child table entries reserved for use only
	 * by mobile nodes.
	 */
	EZSP_CONFIG_RESERVED_MOBILE_CHILD_ENTRIES = 0x0015,


	/**
	 * Enables boost power mode and/or the alternate transmitter output.
	 */
	EZSP_CONFIG_TX_POWER_MODE = 0x0017,

	/**
	 * 0: Allow this node to relay messages. 1: Prevent this
	 * node from relaying messages.
	 */
	EZSP_CONFIG_DISABLE_RELAY = 0x0018,

	/**
	 * The maximum number of EUI64 to network address associations that the Trust Center can
	 * maintain. These address cache entries are reserved for and reused by the Trust Center when
	 * processing device join/rejoin authentications. This cache size limits the number of
	 * overlapping joins the Trust Center can process within a narrow time window (e.g. two
	 * seconds), and thus should be set to the maximum number of near simultaneous joins the Trust
	 * Center is expected to accommodate. (Note, the total number of such address associations
	 * maintained by the NCP is the sum of the value of this setting and the value of
	 * ::EZSP_CONFIG_ADDRESS_TABLE_SIZE.)
	 */
	EZSP_CONFIG_TRUST_CENTER_ADDRESS_CACHE_SIZE = 0x0019,

	/**
	 * The size of the source route table.
	 */
	EZSP_CONFIG_SOURCE_ROUTE_TABLE_SIZE = 0x001A,

	/**
	 * The units used for timing out end devices on their
	 * parents.
	 */
	EZSP_CONFIG_END_DEVICE_POLL_TIMEOUT_SHIFT = 0x001B,

	/**
	 * The number of blocks of a fragmented message that
	 * can be sent in a single window.
	 */
	EZSP_CONFIG_FRAGMENT_WINDOW_SIZE = 0x001C,

	/**
	 * The time the stack will wait (in milliseconds) between
	 * sending blocks of a fragmented message.
	 */
	EZSP_CONFIG_FRAGMENT_DELAY_MS = 0x001D,


	/**
	 * The size of the Key Table used for storing individual link keys (if the device is a Trust
	 * Center) or Application Link Keys (if the device is a normal node).
	 */
	EZSP_CONFIG_KEY_TABLE_SIZE = 0x001E,

	/**
	 * The APS ACK timeout value. The stack waits this
	 * amount of time between resends of APS retried
	 * messages.
	 */
	EZSP_CONFIG_APS_ACK_TIMEOUT = 0x001F,


	/**
	 * The duration of a beacon jitter, in the units used by the
	 * 15.4 scan parameter (((1 << duration) + 1) * 15ms),
	 * when responding to a beacon request.
	 */
	EZSP_CONFIG_BEACON_JITTER_DURATION = 0x0020,

	/**
	 * The time the coordinator will wait (in seconds) for a
	 * second end device bind request to arrive.
	 */
	EZSP_CONFIG_END_DEVICE_BIND_TIMEOUT = 0x0021,

	/**
	 * The number of PAN id conflict reports that must be
	 * received by the network manager within one minute to
	 * trigger a PAN id change.
	 */
	EZSP_CONFIG_PAN_ID_CONFLICT_REPORT_THRESHOLD = 0x0022,

	/**
	 * The timeout value in minutes for how long the Trust
	 * Center or a normal node waits for the ZigBee Request
	 * Key to complete. On the Trust Center this controls
	 * whether or not the device buffers the request, waiting for
	 * a matching pair of ZigBee Request Key. If the value is
	 * non-zero, the Trust Center buffers and waits for that
	 * amount of time. If the value is zero, the Trust Center
	 * does not buffer the request and immediately responds
	 * to the request. Zero is the most compliant behavior.
	 */
	EZSP_CONFIG_REQUEST_KEY_TIMEOUT = 0x0024,

	/**
	 * This value indicates the size of the runtime modifiable
	 * certificate table. Normally certificates are stored in MFG
	 * tokens but this table can be used to field upgrade
	 * devices with new Smart Energy certificates. This value
	 * cannot be set, it can only be queried.
	 */
	EZSP_CONFIG_CERTIFICATE_TABLE_SIZE = 0x0029,

	/**
	 * This is a bitmask that controls which incoming ZDO request messages are passed to the
	 * application. The bits are defined in the EmberZdoConfigurationFlags enumeration. To see
	 * if the application is required to send a ZDO response in reply to an incoming message, the
	 * application must check the APS options bitfield within the incomingMessageHandler
	 * callback to see if the EMBER_APS_OPTION_ZDO_RESPONSE_REQUIRED flag is set.
	 */
	EZSP_CONFIG_APPLICATION_ZDO_FLAGS = 0x002A,

	/**
	 * The maximum number of broadcasts during a single
	 * broadcast timeout period.
	 */
	EZSP_CONFIG_BROADCAST_TABLE_SIZE = 0x002B,

	/**
	 * The size of the MAC filter list table.
	 */
	EZSP_CONFIG_MAC_FILTER_TABLE_SIZE = 0x002C,

	/**
	 * The number of supported networks.
	 */
	EZSP_CONFIG_SUPPORTED_NETWORKS = 0x002D,

	/**
	 * Whether multicasts are sent to the RxOnWhenIdle=true
	 * address (0xFFFD) or the sleepy broadcast address
	 * (0xFFFF). The RxOnWhenIdle=true address is the
	 * ZigBee compliant destination for multicasts.
	 */
	EZSP_CONFIG_SEND_MULTICASTS_TO_SLEEPY_ADDRESS = 0x002E,

	/**
	 * ZLL group address initial configuration.
	 */
	EZSP_CONFIG_ZLL_GROUP_ADDRESSES = 0x002F,

	/**
	 * ZLL rssi threshold initial configuration.
	 */
	EZSP_CONFIG_ZLL_RSSI_THRESHOLD = 0x0030,

	/**
	 * Toggles the mtorr flow control in the stack.
	 */
	EZSP_CONFIG_MTORR_FLOW_CONTROL = 0x0033,

	/**
	 * Setting the retry queue size.
	 */
	EZSP_CONFIG_RETRY_QUEUE_SIZE = 0x0034,

	/**
	 * Setting the new broadcast entry threshold.
	 */
	EZSP_CONFIG_NEW_BROADCAST_ENTRY_THRESHOLD = 0x0035,

	/**
	 * The length of time, in seconds, that a trust center will
	 * store a transient link key that a device can use to join its
	 * network. A transient key is added with a call to
	 * emberAddTransientLinkKey. After the transient key is
	 * added, it will be removed once this amount of time has
	 * passed. A joining device will not be able to use that key
	 * to join until it is added again on the trust center. The
	 * default value is 300 seconds, i.e., 5 minutes.
	 */
	EZSP_CONFIG_TRANSIENT_KEY_TIMEOUT_S = 0x0036,

	/**
	 * The number of passive acknowledgements to record
	 * from neighbors before we stop re-transmitting
	 * broadcasts
	 */
	EZSP_CONFIG_BROADCAST_MIN_ACKS_NEEDED = 0x0037,

	/**
	 * The length of time, in seconds, that a trust center will
	 * allow a Trust Center (insecure) rejoin for a device that is
	 * using the well-known link key. This timeout takes effect
	 * once rejoins using the well-known key has been
	 * allowed. This command updates the
	 * emAllowTcRejoinsUsingWellKnownKeyTimeoutSec
	 * value.
	 */
	EZSP_CONFIG_TC_REJOINS_USING_WELL_KNOWN_KEY_TIMEOUT_S = 0x0038
} EEzspConfigId;

typedef struct {
	unsigned char id;
	unsigned short value;
} SEzspConfig;

typedef enum {
	/**
	 * Controls trust center behavior.
	 */
	EZSP_TRUST_CENTER_POLICY = 0x0000,

	/**
	 * Controls how external binding modification requests are handled.
	 */
	EZSP_BINDING_MODIFICATION_POLICY = 0x0001,

	/**
	 * Controls whether the Host supplies unicast replies.
	 */
	EZSP_UNICAST_REPLIES_POLICY = 0x0002,

	/**
	 * Controls whether pollHandler callbacks are generated.
	 */
	EZSP_POLL_HANDLER_POLICY = 0x0003,

	/**
	 * Controls whether the message contents are included in the messageSentHandler callback.
	 */
	EZSP_MESSAGE_CONTENTS_IN_CALLBACK_POLICY = 0x0004,

	/**
	 * Controls whether the Trust Center will respond to Trust Center link key requests.
	 */
	EZSP_TC_KEY_REQUEST_POLICY = 0x0005,

	/**
	 * Controls whether the Trust Center will respond to application link key requests.
	 */
	EZSP_APP_KEY_REQUEST_POLICY = 0x0006,

	/**
	 * Controls whether ZigBee packets that appear invalid are automatically dropped by the
	 * stack. A counter will be incremented when this occurs.
	 */
	EZSP_PACKET_VALIDATE_LIBRARY_POLICY = 0x0007,

	/**
	 * Controls whether the stack will process ZLL messages.
	 */
	EZSP_ZLL_POLICY = 0x0008,

	/**
	 * Controls whether Trust Center (insecure) rejoins for
	 * devices using the well-known link key are accepted. If
	 * rejoining using the well-known key is allowed, it is
	 * disabled again after
	 * emAllowTcRejoinsUsingWellKnownKeyTimeoutSec
	 * seconds.
	 */
	EZSP_TC_REJOINS_USING_WELL_KNOWN_KEY_POLICY = 0x0009
} EEzspPolicyId;

typedef enum {
	/**
	 * Send the network key in the clear to all joining and rejoining devices.
	 */
	EZSP_ALLOW_JOINS = 0x0000,

	/**
	 * Send the network key in the clear to all joining devices. Rejoining devices are sent the
	 * network key encrypted with their trust center link key. The trust center and any rejoining
	 * device are assumed to share a link key, either preconfigured or obtained under a previous
	 * policy.
	 */
	EZSP_ALLOW_JOINS_REJOINS_HAVE_LINK_KEY = 0x0004,

	/**
	 * Send the network key encrypted with the joining or rejoining device's trust center link key.
	 * The trust center and any joining or rejoining device are assumed to share a link key, either
	 * preconfigured or obtained under a previous policy. This is the default value for the
	 * EZSP_TRUST_CENTER_POLICY.
	 */
	EZSP_ALLOW_PRECONFIGURED_KEY_JOINS = 0x0001,

	/**
	 * Send the network key encrypted with the rejoining device's trust center link key. The trust
	 * center and any rejoining device are assumed to share a link key, either preconfigured or
	 * obtained under a previous policy. No new devices are allowed to join.
	 */
	EZSP_ALLOW_REJOINS_ONLY = 0x0002,

	/**
	 * Reject all unsecured join and rejoin attempts.
	 */
	EZSP_DISALLOW_ALL_JOINS_AND_REJOINS = 0x0003,

	/**
	 * Take no action on trust center rejoin attempts.
	 */
	EZSP_IGNORE_TRUST_CENTER_REJOINS = 0x0005,

	/**
	 * EZSP_BINDING_MODIFICATION_POLICY default decision. Do not allow the local binding table
	 * to be changed by remote nodes.
	 */
	EZSP_DISALLOW_BINDING_MODIFICATION = 0x0010,

	/**
	 * EZSP_BINDING_MODIFICATION_POLICY decision. Allow remote nodes to change the local
	 * binding table.
	 */
	EZSP_ALLOW_BINDING_MODIFICATION = 0x0011,

	/**
	 * EZSP_BINDING_MODIFICATION_POLICY decision. Allows remote nodes to set local binding
	 * entries only if the entries correspond to endpoints defined on the device, and for output
	 * clusters bound to those endpoints.
	 */
	EZSP_CHECK_BINDING_MODIFICATIONS_ARE_VALID_ENDPOINT_CLUSTERS = 0x0012,

	/**
	 * EZSP_UNICAST_REPLIES_POLICY default decision. The NCP will automatically send an empty
	 * reply (containing no payload) for every unicast received.
	 */
	EZSP_HOST_WILL_NOT_SUPPLY_REPLY = 0x0020,

	/**
	 * EZSP_UNICAST_REPLIES_POLICY decision. The NCP will only send a reply if it receives a
	 * sendReply command from the Host.
	 */
	EZSP_HOST_WILL_SUPPLY_REPLY = 0x0021,

	/**
	 * EZSP_POLL_HANDLER_POLICY default decision. Do not inform the Host when a child polls.
	 */
	EZSP_POLL_HANDLER_IGNORE = 0x0030,

	/**
	 * EZSP_POLL_HANDLER_POLICY decision. Generate a pollHandler callback when a child polls.
	 */
	EZSP_POLL_HANDLER_CALLBACK = 0x0031,

	/**
	 * EZSP_MESSAGE_CONTENTS_IN_CALLBACK_POLICY default decision. Include only the message
	 * tag in the messageSentHandler callback.
	 */
	EZSP_MESSAGE_TAG_ONLY_IN_CALLBACK = 0x0040,

	/**
	 * EZSP_MESSAGE_CONTENTS_IN_CALLBACK_POLICY decision. Include both the message tag and
	 * the message contents in the messageSentHandler callback.
	 */
	EZSP_MESSAGE_TAG_AND_CONTENTS_IN_CALLBACK = 0x0041,

	/**
	 * EZSP_TC_KEY_REQUEST_POLICY decision. When the Trust Center receives a request for a Trust
	 * Center link key, it will be ignored.
	 */
	EZSP_DENY_TC_KEY_REQUESTS = 0x0050,

	/**
	 * EZSP_TC_KEY_REQUEST_POLICY decision. When the Trust Center receives a request for a Trust
	 * Center link key, it will reply to it with the corresponding key.
	 */
	EZSP_ALLOW_TC_KEY_REQUESTS = 0x0051,

	/**
	 * EZSP_TC_KEY_REQUEST_POLICY decision. When the Trust Center receives a request for a Trust
	 * Center link key, it will generate a key to send to the joiner.
	 */
	EZSP_GENERATE_NEW_TC_LINK_KEY = 0x0052,

	/**
	 * EZSP_APP_KEY_REQUEST_POLICY decision. When the Trust Center receives a request for an
	 * application link key, it will be ignored.
	 */
	EZSP_DENY_APP_KEY_REQUESTS = 0x0060,

	/**
	 * EZSP_APP_KEY_REQUEST_POLICY decision. When the Trust Center receives a request for an
	 * application link key, it will randomly generate a key and send it to both partners.
	 */
	EZSP_ALLOW_APP_KEY_REQUESTS = 0x0061,

	/**
	 * Indicates that packet validate library checks are enabled on the NCP.
	 */
	EZSP_PACKET_VALIDATE_LIBRARY_CHECKS_ENABLED = 0x0062,

	/**
	 * Indicates that packet validate library checks are NOT enabled on the NCP.
	 */
	EZSP_PACKET_VALIDATE_LIBRARY_CHECKS_DISABLED = 0x0063
} EEzspDecisionId;

typedef struct {
	unsigned char id;
	unsigned char decision;
} SEzspPolicy;


/**
 * @brief Defines the possible join states for a node.
 */
enum {
	/** The node is not associated with a network in any way. */
	EMBER_NO_NETWORK = 0x00,
	/** The node is currently attempting to join a network. */
	EMBER_JOINING_NETWORK,
	/** The node is joined to a network. */
	EMBER_JOINED_NETWORK,
	/** The node is an end device joined to a network but its parent
	    is not responding. */
	EMBER_JOINED_NETWORK_NO_PARENT,
	/** The node is in the process of leaving its current network. */
	EMBER_LEAVING_NETWORK
};

enum {
	/** Unicast sent directly to an EmberNodeId. */
	EMBER_OUTGOING_DIRECT,
	/** Unicast sent using an entry in the address table. */
	EMBER_OUTGOING_VIA_ADDRESS_TABLE,
	/** Unicast sent using an entry in the binding table. */
	EMBER_OUTGOING_VIA_BINDING,
	/** Multicast message.  This value is passed to emberMessageSentHandler() only.
	 * It may not be passed to emberSendUnicast(). */
	EMBER_OUTGOING_MULTICAST,
	/** Broadcast message.  This value is passed to emberMessageSentHandler() only.
	 * It may not be passed to emberSendUnicast(). */
	EMBER_OUTGOING_BROADCAST
};

typedef enum {
	/** Unicast. */
	EMBER_INCOMING_UNICAST,
	/** Unicast reply. */
	EMBER_INCOMING_UNICAST_REPLY,
	/** Multicast. */
	EMBER_INCOMING_MULTICAST,
	/** Multicast sent by the local device. */
	EMBER_INCOMING_MULTICAST_LOOPBACK,
	/** Broadcast. */
	EMBER_INCOMING_BROADCAST,
	/** Broadcast sent by the local device. */
	EMBER_INCOMING_BROADCAST_LOOPBACK,
	/** Many to one route request. */
	EMBER_INCOMING_MANY_TO_ONE_ROUTE_REQUEST
} EmberIncomingMessageType;

constexpr uint8_t EZSP_ENERGY_SCAN = 0x00;
constexpr uint8_t EZSP_ACTIVE_SCAN = 0x01;

typedef enum {
	/**
	 * Normally devices use MAC Association to join a network, which
	 * respects the "permit joining" flag in the MAC Beacon. This value
	 * should be used by default.
	 */
	EMBER_USE_MAC_ASSOCIATION = 0x00,
	/**
	 * For those networks where the "permit joining" flag is never turned
	 * on, they will need to use a ZigBee NWK Rejoin. This value
	 * causes the rejoin to be sent without NWK security and the Trust
	 * Center will be asked to send the NWK key to the device. The
	 * NWK key sent to the device can be encrypted with the device's
	 * corresponding Trust Center link key. That is determined by the
	 * ::EmberJoinDecision on the Trust Center returned by the
	 * ::emberTrustCenterJoinHandler().
	 */
	EMBER_USE_NWK_REJOIN = 0x01,
	/**
	 * For those networks where the "permit joining" flag is never turned
	 * on, they will need to use a NWK Rejoin. If those devices have
	 * been preconfigured with the NWK key (including sequence
	 * number) they can use a secured rejoin. This is only necessary for
	 * end devices since they need a parent. Routers can simply use
	 * the ::EMBER_USE_NWK_COMMISSIONING join method below.
	 */
	EMBER_USE_NWK_REJOIN_HAVE_NWK_KEY = 0x02,
	/**
	 * For those networks where all network and security information is
	 * known ahead of time, a router device may be commissioned such
	 * that it does not need to send any messages to begin
	 * communicating on the network.
	 */
	EMBER_USE_NWK_COMMISSIONING = 0x03
} EmberJoinMethod;

constexpr uint16_t INVALID_NODE_ID = 0xFFFF;
typedef uint16_t EmberNodeId;

typedef enum {
	/**
	 * Device is not joined.
	 */
	EMBER_UNKNOWN_DEVICE = 0x00,
	/**
	 * Will relay messages and can act as a parent to other nodes.
	 */
	EMBER_COORDINATOR = 0x01,
	/**
	 * Will relay messages and can act as a parent to other nodes.
	 */
	EMBER_ROUTER = 0x02,
	/**
	 * Communicates only with its parent and will not relay messages.
	 */
	EMBER_END_DEVICE = 0x03,
	/**
	 * An end device whose radio can be turned off to save power. The
	 * application must poll to receive messages.
	 */
	EMBER_SLEEPY_END_DEVICE = 0x04
} EmberNodeType;


typedef enum {
	E_OUT_MSG_BR_DEST_ALL_DEVICES = 0xFFFF,
	E_OUT_MSG_BR_DEST_NON_SLEEPY = 0xFFFD,
	E_OUT_MSG_BR_DEST_ROUTER_ONLY = 0xFFFC
} EOutBroadcastDestination;

typedef enum {
	EMBER_TRUST_CENTER_LINK_KEY = 0x01,
	EMBER_CURRENT_NETWORK_KEY = 0x03,
	EMBER_NEXT_NETWORK_KEY = 0x04,
	EMBER_APPLICATION_LINK_KEY = 0x05
} EmberKeyType;

typedef enum {
	EMBER_KEY_HAS_SEQUENCE_NUMBER = 0x0001,
	EMBER_KEY_HAS_OUTGOING_FRAME_COUNTER = 0x0002,
	EMBER_KEY_HAS_INCOMING_FRAME_COUNTER = 0x0004,
	EMBER_KEY_HAS_PARTNER_EUI64 = 0x0008
} EmberKeyStructBitmask;

typedef enum {
	EZSP_DECISION_BITMASK_DEFAULT_CONFIGURATION = 0x0000,
	EZSP_DECISION_ALLOW_JOINS = 0x0001,
	EZSP_DECISION_ALLOW_UNSECURED_REJOINS = 0x0002,
	EZSP_DECISION_SEND_KEY_IN_CLEAR = 0x0004,
	EZSP_DECISION_IGNORE_UNSECURED_REJOINS = 0x0008,
	EZSP_DECISION_JOINS_USE_INSTALL_CODE_KEY = 0x0010,
	EZSP_DECISION_DEFER_JOINS = 0x0020
} EzspDecisionBitmask;

constexpr int EMBER_KEY_DATA_BYTE_SIZE = 16;
typedef std::array<uint8_t, EMBER_KEY_DATA_BYTE_SIZE> EmberKeyData;

constexpr int EMBER_EUI64_BYTE_SIZE = 8;
typedef std::array<uint8_t, EMBER_EUI64_BYTE_SIZE> EmberEUI64;

typedef enum {
	DONGLE_READY,
	DONGLE_REMOVE,
	DONGLE_NOT_RESPONDING,
	DONGLE_VERSION_RETRIEVED,
} EDongleState;

class CEzspEnum {
public:
	static std::string EmberNodeTypeToString( EmberNodeType in );
	static std::string EEmberStatusToString( EEmberStatus in );
	static std::string EmberJoinMethodToString( EmberJoinMethod in );
	static std::string EEzspCmdToString( EEzspCmd in );
	static std::string EmberKeyTypeToString( EmberKeyType in );
	static std::string EmberIncomingMessageTypeToString( EmberIncomingMessageType in );
};

// The security level of the GPD.
typedef uint8_t EmberGpSecurityLevel;

// The type of security key to use for the GPD.
typedef uint8_t EmberGpKeyType;

// The proxy table entry status
typedef uint8_t EmberGpProxyTableEntryStatus;

// The security frame counter
typedef uint32_t EmberGpSecurityFrameCounter;

// The sink table entry status
typedef uint8_t EmberGpSinkTableEntryStatus;

constexpr int GP_SINK_LIST_ENTRIES = 2; // hardcoded to 2 which is the spec minimum (cf. A.3.4.2.2.6 Sink group list parameter from doc-14-0563-16-batt-green-power-spec_ProxyBasic.pdf)
constexpr int EMBER_GP_SINK_LIST_ENTRY_SIZE = 11; // why ? first byte to 0xFF is for not used !
typedef std::vector<uint8_t> EmberGpSinkListEntry; // FIXME: sould change to typedef std::array<uint8_t, EMBER_GP_SINK_LIST_ENTRY_SIZE> EmberGpSinkListEntry;

// assume this value is never reach for a frame counter
constexpr uint32_t INVALID_FRAME_COUNTER = 0xFFFFFFFFU;

} // namespace NSEZSP
