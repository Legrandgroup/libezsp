/**
 * @file green-power-sink.h
 *
 * @brief Access to green power capabilities
 */

#pragma once

#include <map>
#include <vector>

#include "ezsp/zbmessage/green-power-frame.h"
#include "ezsp/zbmessage/green-power-device.h"
#include "ezsp/green-power-observer.h"
#include "ezsp/ezsp-dongle.h"
#include "ezsp/zigbee-tools/zigbee-messaging.h"
#ifdef USE_BUILTIN_MIC_PROCESSING
#include "ezsp/zigbee-tools/green-power-device-db.h"
#endif
#include "ezsp/ezsp-protocol/struct/ember-gp-sink-table-entry-struct.h"
#include "ezsp/ezsp-protocol/struct/ember-process-gp-pairing-parameter.h"
#include "ezsp/ezsp-protocol/struct/ember-network-parameters.h"
#include "spi/ByteBuffer.h"
#include "ezsp/enum-generator.h"

namespace NSEZSP {

#define SINK_STATE(XX) \
	XX(SINK_NOT_INIT,=1)                 /*<! Initializing state */ \
	XX(SINK_READY,)                      /*<! Default state if no action in progress */ \
	XX(SINK_ERROR,)                      /*<! Something went wrong */ \
	XX(SINK_COM_OPEN,)                   /*<! GP Proxy Commissioning mode open */ \
	XX(SINK_COM_IN_PROGRESS,)            /*<! GP sink received gpf commissionning frame */ \
	XX(SINK_COM_OFFLINE_IN_PROGRESS,)    /*<! Doing offline commissionning based on a pre-known GPD source ID+key list (stored in gpds_to_register) */ \
	XX(SINK_AUTHORIZE_ANSWER_CH_RQST,)   /*<! Allowing answer to channel request maintenance green power frame */ \
	XX(SINK_CLEAR_ALL,)                  /*<! Currently clearing all tables (sink/proxy) */ \
	XX(SINK_REMOVE_IN_PROGRESS,)         /*<! Currently removing, from the sink and proxy tables, data matching with a specific list of GPDS (stored in gpds_to_remove) */ \

class CGpSink : public CEzspDongleObserver {
public:
	/**
	 * @brief Current state for the GP sink
	 *
	 * @note The lines above describes all states known in order to build both an enum and enum-to-string/string-to-enum methods
	 *       In this macro, XX is a placeholder for the macro to use for building.
	 *       We start numbering from 1, so that 0 can be understood as value not found for enum-generator.h
	 * @see enum-generator.h
	 */
	DECLARE_ENUM(State, SINK_STATE);

	CGpSink( CEzspDongle &i_dongle, CZigbeeMessaging &i_zb_messaging );

	CGpSink() = delete; /* Construction without arguments is not allowed */
	CGpSink(const CGpSink&) = delete; /* No copy construction allowed */

	CGpSink& operator=(CGpSink) = delete; /* No assignment allowed */

	/**
	 * @brief Initialize sink, shall be done after a network init.
	 */
	void init();

	/**
	 * @brief Clear all GP tables
	 *
	 * @return true if action can be done
	 */
	bool gpClearAllTables();

	/**
	 * @brief Open a commissioning session for limited time, close as soon as a binding is done.
	 */
	void openCommissioningSession();

	/**
	 * @brief Force to close commissioning session
	 */
	void closeCommissioningSession();

	/**
	 * @brief Remove all previously configured green power device to this sink
	 *
	 * @return true if action can be done
	 */
	bool clearAllGpds();

	/**
	 * @brief Add a green power device to this sink
	 *
	 * @param gpd list of gpds to add
	 *
	 * @return true if action can be done
	 */
	bool registerGpds(std::vector<CGpDevice> gpd);

	/**
	 * @brief remove a green power device to this sink
	 *
	 * @param gpd list of gpds sourceId to remove
	 *
	 * @return true if action can be done
	 */
	bool removeGpds(std::vector<uint32_t> gpd);

	/**
	 * @brief authorize answer to channel request
	 *
	 * @param i_authorize : true to authorize, false otherwize
	 */
	void authorizeAnswerToGpfChannelRqst( bool i_authorize ) {
		authorizeGpfChannelRqst = i_authorize;
	}

	/**
	 * @brief Method that will be invoked on incoming EZSP messages
	 *
	 * @param i_cmd The EZSP command
	 * @param i_msg_receive The payload of the message
	 */
	void handleEzspRxMessage( EEzspCmd i_cmd, NSSPI::ByteBuffer i_msg_receive );

	/**
	 * @brief Register one observer to the sink events
	 *
	 * @param[in] observer The observer to register
	 *
	 * @return true if The observer could successfully be registered
	 */
	bool registerObserver(CGpObserver* observer);

	/**
	 * @brief Un-register one observer to the sink events
	 *
	 * @param[in] observer The observer to un-register
	 *
	 * @return true if The observer could successfully be un-registered
	 */
	bool unregisterObserver(CGpObserver* observer);

	/**
	 * @brief Register callback on current library state
	 */
	void registerStateCallback(std::function<bool (CGpSink::State& i_state)> newObsStateCallback);

private:
	/**
	 * @brief Notify observers of this class
	 *
	 * @param i_gpf The received GP frame
	 */
	void notifyObserversOfRxGpFrame( CGpFrame i_gpf );

	/**
	 * @brief Notify observers of this class
	 *
	 * @param i_gpd_id The received GPD Id frame
	 */
	void notifyObserversOfRxGpdId( uint32_t i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status );

	/**
	 * @brief Notify observers of this class
	 *
	 * @param i_gpd_id The received GPD commissioning frame
	 */
	void notifyObserversForCommissioningGpDevice( CGpFrame i_gpf );

	/**
	 * @brief Private utility function to manage error state
	 */
	void setSinkState(CGpSink::State i_state);

	/**
	 * @brief send zigbee unicast message GP Proxy Commissioning Mode.
	 *        done from sink to local dongle.
	 * @warning All parameters are hardcoded for testing
	 */
	void sendLocalGPProxyCommissioningMode(uint8_t i_option);

	/**
	 * @brief Retrieves the sink table entry stored at the specified index
	 *
	 * @param i_index The index to lookup
	 */
	void gpSinkGetEntry( uint8_t i_index );

	/**
	 * @brief Finds or allocates a sink entry
	 *
	 * @note You should make sure the sink was in SINK_READY state before invoking this private method
	 *
	 * @param i_src_id GPD source ID address to search
	 */
	void gpSinkTableFindOrAllocateEntry( uint32_t i_src_id );

	/**
	 * @brief Updates the sink table entry at the specified index.
	 *
	 * @param i_index The index of the requested sink table entry.
	 * @param i_entry An EmberGpSinkTableEntry struct containing a copy of the sink entry to be updated.
	 */
	void gpSinkSetEntry( uint8_t i_index, CEmberGpSinkTableEntryStruct& i_entry );

	/**
	 * @brief Update the GP Proxy table based on a GP pairing.
	 */
	void gpProxyTableProcessGpPairing(const CProcessGpPairingParam& i_param);

	/**
	 * @brief
	 *
	 * @param i_action The action to perform on the GP TX queue (true to add, false to remove).
	 * @param i_use_cca Whether to use ClearChannelAssessment when transmitting the GPDF.
	 * @param i_gp_addr The Address of the destination GPD.
	 * @param i_gpd_command_id The GPD command ID to send.
	 * @param[in] i_gpd_command_payload The GP command payload.
	 * @param i_life_time_ms How long to keep the GPDF in the TX Queue.
	 * @param i_handle An handle value for this frame, use to identify in sent callback.
	 *
	 */
	void gpSend(bool i_action, bool i_use_cca, CEmberGpAddressStruct i_gp_addr,
	            uint8_t i_gpd_command_id, const NSSPI::ByteBuffer& i_gpd_command_payload, uint16_t i_life_time_ms, uint8_t i_handle=0 );

	/**
	 * @brief Remove an entry in sink table
	 * @param i_index The index of the entry to remove
	 */
	void gpSinkTableRemoveEntry( uint8_t i_index );

	/**
	 * @brief Search an entry in the proxy table for a GPD, using a source ID as key
	 * @param i_src_id Source id of GPD to search
	 */
	void gpProxyTableLookup(uint32_t i_src_id);

	/**
	 * @brief Search an entry in the sink table for a GPD, using a source ID as key
	 * @param i_src_id Source id of GPD to search
	 */
	void gpSinkTableLookup(uint32_t i_src_id);

	/**
	 * @brief Handle an incoming GET_NETWORK_PARAMETERS EZSP message
	 * @param[in] i_msg_receive The incoming EZSP message
	 */
	void handleEzspRxMessage_GET_NETWORK_PARAMETERS(const NSSPI::ByteBuffer& i_msg_receive);

	/**
	 * @brief Handle an incoming EZSP message related to the Green Power endpoint with no GPD security
	 * @param[in] gpf The Green Power frame
	 */
	void handleEzspRxMessage_INCOMING_MESSAGE_HANDLER_NO_SECURITY(const CGpFrame& gpf);

	/**
	 * @brief Handle an incoming EZSP message related to the Green Power endpoint with GPD security
	 * @param[in] gpf The Green Power frame
	 */
	void handleEzspRxMessage_INCOMING_MESSAGE_HANDLER_SECURITY(const CGpFrame& gpf);

	/**
	 * @brief Handle an incoming EZSP message related to the Green Power endpoint
	 * @param[in] i_msg_receive The incoming EZSP message
	 *
	 * @note This method will then dispatch to either handleEzspRxMessage_INCOMING_MESSAGE_HANDLER_NO_SECURITY() or handleEzspRxMessage_INCOMING_MESSAGE_HANDLER_SECURITY()
	 */
	void handleEzspRxMessage_INCOMING_MESSAGE_HANDLER(const NSSPI::ByteBuffer& i_msg_receive);

	/**
	 * @brief Handle an incoming SINK_TABLE_FIND_OR_ALLOCATE_ENTRY EZSP message
	 * @param[in] i_msg_receive The incoming EZSP message
	 */
	void handleEzspRxMessage_SINK_TABLE_FIND_OR_ALLOCATE_ENTRY(const NSSPI::ByteBuffer& i_msg_receive);

	/**
	 * @brief Handle an incoming SINK_TABLE_LOOKUP EZSP message
	 * @param[in] i_msg_receive The incoming EZSP message
	 */
	void handleEzspRxMessage_SINK_TABLE_LOOKUP(const NSSPI::ByteBuffer& i_msg_receive);

	/**
	 * @brief Handle an incoming SINK_TABLE_GET_ENTRY EZSP message
	 * @param[in] i_msg_receive The incoming EZSP message
	 */
	void handleEzspRxMessage_SINK_TABLE_GET_ENTRY(const NSSPI::ByteBuffer& i_msg_receive);

	/**
	 * @brief Handle an incoming SINK_TABLE_SET_ENTRY EZSP message
	 * @param[in] i_msg_receive The incoming EZSP message
	 */
	void handleEzspRxMessage_SINK_TABLE_SET_ENTRY(const NSSPI::ByteBuffer& i_msg_receive);

	/**
	 * @brief Handle an incoming PROXY_TABLE_LOOKUP EZSP message
	 * @param[in] i_msg_receive The incoming EZSP message
	 */
	void handleEzspRxMessage_PROXY_TABLE_LOOKUP(const NSSPI::ByteBuffer& i_msg_receive);

	/**
	 * @brief Handle an incoming PROXY_TABLE_GET_ENTRY EZSP message
	 * @param[in] i_msg_receive The incoming EZSP message
	 */
	void handleEzspRxMessage_PROXY_TABLE_GET_ENTRY(const NSSPI::ByteBuffer& i_msg_receive);

	/**
	 * @brief Handle an incoming PROXY_TABLE_PROCESS_GP_PAIRING EZSP message
	 * @param[in] i_msg_receive The incoming EZSP message
	 */
	void handleEzspRxMessage_PROXY_TABLE_PROCESS_GP_PAIRING(const NSSPI::ByteBuffer& i_msg_receive);

private:
	CEzspDongle &dongle; /*!< The EZSP adapter used to send/receive EZSP commands */
	CZigbeeMessaging &zb_messaging;     /*!< A CZigbeeMessaging object used to send unicast Zigbee messages */
	CGpSink::State sink_state;  /*!< Current state for our internal state machine */
	std::function<bool (CGpSink::State& i_state)> obsStateCallback;	/*!< Optional user callback invoked by us each time library state change */
	CEmberNetworkParameters nwk_parameters;
	bool authorizeGpfChannelRqst;
	// parameters to save for pairing/clearing
	CGpFrame gpf_comm_frame;
	uint8_t sink_table_index;
	std::vector<CGpDevice> gpds_to_register;    /*!< A list of GP source IDs and keys to register to the adapter's GP sink */
	CEmberGpSinkTableEntryStruct sink_table_entry;
	uint8_t proxy_table_index;
	std::vector<uint32_t> gpds_to_remove;    /*!< A list of GP source IDs to remove from the adapter's GP sink */
	std::mutex gpdSentStateMutex;	/*!< Mutex allowing exclusive accesses to gpdSentPendingAckList and gpdSentLastHandlerNb */
	uint8_t gpdSentLastHandlerNb;	/*!< The value of the last handler number used when invoking to dGpSend(), this will be returned by the adapter when the frame is acknowledged as sent */
	uint32_t sentChannelSwitchSourceId;	/*!< If non-0, this attribute contains the source ID of an MSP channel switch request already sent but not yet successfully acknowledged by the adapter */
	std::set<CGpObserver*> observers;   /*!< List of observers of this class */
#ifdef USE_BUILTIN_MIC_PROCESSING
	NSEZSP::CGPDeviceDb gp_dev_db;    /*!< A database of known Green Power devices */
#endif
};

} // namespace NSEZSP
