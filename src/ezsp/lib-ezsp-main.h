/**
 * @file lib-ezsp-main.h
 *
 * @brief Main API methods for libezsp
 */

#pragma once

#include <map>

#include "spi/IUartDriver.h"
#include "spi/TimerBuilder.h"

#include <ezsp/ezsp.h>
#include "ezsp/enum-generator.h"
#include "ezsp/ezsp-dongle.h"
#include "ezsp/zigbee-tools/zigbee-networking.h"
#include "ezsp/zbmessage/zdp-enum.h"
#include "ezsp/zigbee-tools/zigbee-messaging.h"
#include "ezsp/zigbee-tools/green-power-sink.h"
#include "ezsp/zbmessage/green-power-device.h"
#include "ezsp/ezsp-protocol/struct/ember-zigbee-network.h"	// For CEmberZigbeeNetwork
#include "spi/ByteBuffer.h"

#include "ezsp/ezsp-dongle-observer.h"
#include "ezsp/green-power-observer.h"

namespace NSEZSP {

#define CLIBEZSP_INTERNAL_STATE_LIST(XX) \
	XX(UNINITIALIZED,=1)                    /*<! Initial state, before starting */ \
	XX(WAIT_DONGLE_READY,)                  /*<! Waiting for the dongle to be ready */ \
	XX(GETTING_EZSP_VERSION,)               /*<! Inside the EZSP version matching loop */ \
	XX(GETTING_XNCP_INFO,)                  /*<! Inside the XNCP info check */ \
	XX(STACK_INIT,)                         /*<! We are starting up the Zigbee stack in the adapter */ \
	XX(FORM_NWK_IN_PROGRESS,)               /*<! We are currently creating a new Zigbee network */ \
	XX(LEAVE_NWK_IN_PROGRESS,)              /*<! We are currently leaving the Zigbee network we previously joined */ \
	XX(READY,)                              /*<! Library is ready to work and process new command */ \
	XX(SCANNING,)                           /*<! An network scan in currently being run */ \
	XX(JOINING,)                            /*<! A network join is pending */ \
	XX(INIT_FAILED,)                        /*<! Initialisation failed, Library is out of work */ \
	XX(SINK_BUSY,)                          /*<! Enclosed sink is busy executing commands */ \
	XX(SWITCHING_TO_BOOTLOADER_MODE,)       /*<! Switch to bootloader is pending */ \
	XX(IN_BOOTLOADER_MENU,)                 /*<! We are on the bootloader menu prompt */ \
	XX(IN_XMODEM_XFR,)                      /*<! We are currently doing X-Modem transfer */ \
	XX(SWITCHING_TO_EZSP_MODE,)             /*<! Switch to EZSP mode (normal mode) is pending */ \
	XX(SWITCH_TO_BOOTLOADER_IN_PROGRESS,)   /*<! We are currently starting bootloader more */ \
	XX(TERMINATING,)                        /*<! We are shutting down the library */ \

/**
 * @brief Internal states for CLibEzspMain
 *
 * Not all these states are exposed to the outside, only CLibEzspState states (and the related changes) are notified
 * A mapping between CLibEzspInternalState and CLibEzspMain can be found in method setState()
 *
 * @note The lines above describes all states known in order to build both an enum and enum-to-string/string-to-enum methods
 *       In this macro, XX is a placeholder for the macro to use for building.
 *       We start numbering from 1, so that 0 can be understood as value not found for enum-generator.h
 * @see enum-generator.h
 */
class CLibEzspInternal {
public:
	DECLARE_ENUM(State, CLIBEZSP_INTERNAL_STATE_LIST);
};

/**
 * @brief Class allowing sending commands and receiving events from an EZSP interface
 */
class CLibEzspMain : public CEzspDongleObserver, CGpObserver {
public:
	/**
	 * @brief Default constructor with minimal args to initialize library
	 *
	 * @param uartHandle A handle on a IUartDriver instance to send/receive EZSP message over a serial line
	 * @param timerbuilder An ITimerFactory used to generate ITimer objects
	 * @param requestZbNetworkResetToChannel Set this to non 0 if we should destroy any pre-existing Zigbee network in the EZSP adapter and recreate a new Zigbee network on the specified 802.15.4 channel number, set this to -1 if we should just leave any previously joined network
	 */
	CLibEzspMain(NSSPI::IUartDriverHandle uartHandle, const NSSPI::TimerBuilder& timerbuilder, unsigned int requestZbNetworkResetToChannel);

	CLibEzspMain() = delete; /*<! Construction without arguments is not allowed */
	CLibEzspMain(const CLibEzspMain&) = delete; /*<! No copy construction allowed */
	CLibEzspMain& operator=(CLibEzspMain) = delete; /*<! No assignment allowed */

	/**
	 * @brief Startup the EZSP adapter
	 *
	 * @note Calling this method is required after instanciation and before any data is sent to/received from the EZSP adatper
	 */
	void start();

	/**
	 * @brief Retrieve an observable to handle bytes received on the serial port
	 *
	 * @note This observable has been created at construction (see our constructor's uartHandler argument) and set as the observable for this uartHandler
	 * Thus, if another observable is setconfigured on uartHandler, the CEzsp instance will not receive incoming bytes anymore.
	 * To allow an external observer to attach to this observable and thus get the incoming serial bytes as well, we provide this utility
	 * method to expose the observable we created
	 *
	 * @return An observable instance that will notify its observers when new bytes are read from the serial port
	 */
	NSSPI::GenericAsyncDataInputObservable* getAdapterSerialReadObservable();

	/**
	 * @brief Get the EZSP adapter's version
	 *
	 * @return The EZSP adapter version (hardware and firmware) if already known
	 */
	NSEZSP::EzspAdapterVersion getAdapterVersion() const;

	/**
	 * @brief Register callback on current library state
	 *
	 * @param newObsGPFrameRecvCallback A callback function that will be invoked each time a new valid green power frame is received from a known source ID (or nullptr to disable this callback)
	 */
	void registerLibraryStateCallback(FLibStateCallback newObsStateCallback);

	/**
	 * @brief Register callback to receive all zcl incoming frames
	 *
	 * @param newObsZclFrameRecvCallback A callback function that will be invoked each time a new zcl frame is received from a known source ID
	 */
	void registerZclFrameRecvCallback(FZclFrameRecvCallback newObsZclFrameRecvCallback);

	/**	
	 * @brief Register callback to receive the bindings table on specific node id
	 *
	 * @param newObsBindingTableRecvCallback A callback function that will be invoked each time a new bindings table is received from a known source ID
	 */
	void registerBindingTableRecvCallback(FBindingTableRecvCallback newObsBindingTableRecvCallback);

	/**
	 * @brief Register callback to receive join/leave network
	 *
	 * @param newObsTrustCenterJoinHandlerCallback A callback function that will be invoked each time a EZSP_TRUST_CENTER_JOIN_HANDLER is received
	 */
	void registerTrustCenterJoinHandlerCallback(FTrustCenterJoinHandlerCallBack newObsTrustCenterJoinHandlerCallback);

	/**
	 * @brief Register callback to receive new node id in network
	 *
	 * @param newObsZdpDeviceAnnounceRecvCallback A callback function that will be invoked each time a new device join the network
	 */
	void registerZdpDeviceAnnounceRecvCallback(FZdpDeviceAnnounceCallBack newObsZdpDeviceAnnounceRecvCallback);

	/**
	 * @brief Register callback to receive simple descriptor on specific endpoint
	 *
	 * @param newObsZdpSimpleDescRecvCallback A callback function that will be invoked each time a ZDP_SIMPLE_DESC is received
	 */
	void registerZdpSimpleDescRecvCallback(FZdpSimpleDescCallBack newObsZdpSimpleDescRecvCallback);

	/**
	 * @brief Register callback to get EUI64 dongle
	 *
	 * @param newObsDongleEUI64RecvCallback A callback function that will be invoked each time a EZSP_GET_EUI64 is received
	 */
	void registerDongleEUI64RecvCallback(FDongleEUI64CallBack newObsDongleEUI64RecvCallback);

	/**
	 * @brief Register callback to receive active endpoint on specific node id
	 *
	 * @param newObsZdpActiveEpRecvCallback A callback function that will be invoked each time a ZDP_ACTIVE_ENDPOINT is received
	 */
	void registerZdpActiveEpRecvCallback(FZdpActiveEpCallBack newObsZdpActiveEpRecvCallback);

	/**
	 * @brief Register callback to receive all authenticated incoming green power frames
	 *
	 * @param newObsGPFrameRecvCallback A callback function of type void func(CGpFrame &i_gpf), that will be invoked each time a new valid green power frame is received from a known source ID (or nullptr to disable this callback)
	 */
	void registerGPFrameRecvCallback(FGpFrameRecvCallback newObsGPFrameRecvCallback);

	/**
	 * @brief Register callback to receive all incoming green power sourceId
	 *
	 * @param newObsGPSourceIdCallback A callback function that will be invoked each time a new source ID transmits over the air (or nullptr to disable this callback)
	 */
	void registerGPSourceIdCallback(FGpSourceIdCallback newObsGPSourceIdCallback);

	/**
	 * @brief Get EUI64 dongle
	 *
	 * @return true if the action is going to be run in the background, false if the sink is busy
	 */
	bool getEUI64();

	/**
	 * @brief Remove GP all devices from sink
	 *
	 * After invoking this method, we won't process any message from any GP devices (but registerGPSourceIdCallback will still be triggered)
	 *
	 * @return true if the action is going to be run in the background, false if the sink is busy
	 */
	bool clearAllGPDevices();

	/**
	 * @brief Remove a specific set of GP devices from sink (identified by their source ID value)
	 *
	 * After invoking this method, we won't process any message from the GP devices provided as argument (but registerGPSourceIdCallback will still be triggered)
	 *
	 * @param sourceIdList A list of source IDs to remove
	 *
	 * @return true if the action is going to be run in the background, false if the sink is busy
	 */
	bool removeGPDevices(const std::vector<uint32_t>& sourceIdList);

	/**
	 * @brief Add a specific set of GP devices from sink (identified by their source ID value+key)
	 *
	 * After invoking this method, we will start taking into acocunt all messages from the GP devices provided as argument
	 *
	 * @param gpDevicesList A list of CGpDevice objects containing source ID + key pairs to add
	 *
	 * @return true if the action is going to be run in the background, false if the sink is busy
	 */
	bool addGPDevices(const std::vector<CGpDevice> &gpDevicesList);

	/**
	 * @brief Open a Green Power commissionning session
	 *
	 * After invoking this method, we will accept new Green Power Devices to become bound to us
	 *
	 * @return true if the action is going to be run in the background, false if the sink is busy
	 */
	bool openCommissioningSession();

	/**
	 * @brief Close a Green Power commissionning session
	 *
	 * If the commissionning session was previously opened openCommissioningSession(), ater invoking this method, we will not accept new Green Power Devices anymore
	 *
	 * @return true if the action is accepted, false if the sink is busy
	 */
	bool closeCommissioningSession();

	/**
	 * @brief Controls the answer to request channel messages sent by GP devices
	 *
	 * @param allowed Set to true if answers to request channel is allowed
	 */
	void setAnswerToGpfChannelRqstPolicy(bool allowed);

	/**
	 * @brief Makes initialization timeout trigger a switch to firmware upgrade mode
	 *
	 * Default behaviour for initialization timeouts is to probe the bootloader prompt and if found, to run the EZSP application
	 * in the hope the adapter will move back to EZSP mode
	 */
	void forceFirmwareUpgradeOnInitTimeout();

	/**
	 * @brief Switch the EZSP adapter to firmware upgrade mode
	 *
	 * Method handleFirmwareXModemXfr() should then be invoked when the adapter is ready to receive a firmware
	 */
	void setFirmwareUpgradeMode();

	/**
	 * @brief Start an energy scan on the EZSP adapter
	 *
	 * When the scan is complete, a EZSP_ENERGY_SCAN_RESULT_HANDLER EZSP message will be received from the adapter
	 *
	 * @param energyScanCallback A callback function of type void func(std::map<uint8_t, int8_t>) that will be invoked when the energy scan is finished.
	 *                           The map provided to the callback contains entries with the key (uint8_t) being the 802.15.4 channel, and the value (int8_t) being the measured RSSI on this channel
	 * @param duration The exponent of the number of scan periods, where a scan period is 960 symbols. The scan will occur for ((2^duration) + 1) scan periods((2^duration) + 1) scan periods
	 * @param requestedChannelMask A mask of channels to scan (for example, to scan channels 11, 16 and 25, the mask would be 1<<11|1<<16|1<<25, providing 0 here means all channels
	 *
	 * @return true if the scan could be started, false otherwise (adapter is not ready, maybe a scan is already ongoing)
	 */
	bool startEnergyScan(FEnergyScanCallback energyScanCallback, uint8_t duration = 3, uint32_t requestedChannelMask = 0);

	/**
	 * @brief Start an active scan on the EZSP adapter
	 *
	 * When the scan is complete, a EZSP_ENERGY_SCAN_RESULT_HANDLER EZSP message will be received from the adapter
	 *
	 * @param activeScanCallback A callback function of type void func(std::map<uint8_t, std::vector<NSEZSP::ZigbeeNetworkScan>>) that will be invoked when the active scan is finished.
	 *                           The map provided to the callback contains entries with the key (uint8_t) being the 802.15.4 channel, and the value (std::set<NSEZSP::ZigbeeNetworkScan>) being at set with descriptions of all zigbee networks found on that channel
	 * @param duration The exponent of the number of scan periods, where a scan period is 960 symbols. The scan will occur for ((2^duration) + 1) scan periods((2^duration) + 1) scan periods
	 *                 The default value (3) allows for a quite fast scan. Values above 6 may result in longer scan duration.
	 * @param requestedChannelMask A mask of channels to scan (for example, to scan channels 11, 16 and 25, the mask would be 1<<11|1<<16|1<<25, providing 0 here means all channels
	 *
	 * @return true If the scan could be started, false otherwise (adapter is not ready, maybe a scan is already ongoing)
	 */
	bool startActiveScan(FActiveScanCallback activeScanCallback, uint8_t duration = 3, uint32_t requestedChannelMask = 0);

	/**
	 * @brief Get the value of the current network encryption key
	 *
	 * @param networkKeyCallback A callback function of type void func(EEmberStatus status, const CEmberKeyStruct& key) that will be invoked with the result of the query.
	 *                           The first argument of the callback is an EEmberStatus indicating whether the request was successful or not
	 *                           The second argument of the callback is a CEmberKeyStruct containing the network key details (only valid if EEmberStatus is set to EEmberStatus::EMBER_SUCCESS)
	 *
	 * @return true if the action was taken into account, false otherwise (adapter is not ready)
	 */
	bool getNetworkKey(FNetworkKeyCallback networkKeyCallback);

	/**
	 * @brief Select the 802.15.4 channel on which the EZSP adapter works
	 *
	 * @param channel The 802.15.4 channel (valid values are 11 to 26, inclusive)
	 *
	 * @return true If the channel could be set
	 */
	bool setChannel(uint8_t channel);

	/**
	 * @brief Join a zigbee network
	 *
	 * Causes the stack to associate with the network using the specified network parameters. It can take several seconds for the stack
	 * to associate with the local network. Do not send messages until the stack is up.
	 *
	 * @return true If the join action could be started
	 */
	bool joinNetwork(NSEZSP::CEmberNetworkParameters& nwkParams);

	/**
	 * @brief Create a zigbee network
	 *
	 * @param channel The 802.15.4 channel (valid values are 11 to 26, inclusive)
	 *
	 * @return true If the join action could be started
	 */
	bool createNetwork(uint8_t channel);

	/**
	 * @brief Open the zigbee network for a defined period so other products can join the zigbee network
	 *
	 * @param i_timeout The time during the network is open
	 *
	 * @return true If the join action could be started
	 */
	bool openNetwork(uint8_t i_timeout);

	/**
	 * @brief Send a ZDO unicast command
	 *
	 * @param i_node_id Short address of destination
	 * @param i_cmd_id Command
	 * @param[in] payload Payload for the ZDO unicast
	 *
	 * @return true if message can be send
	 */
	bool SendZDOCommand(EmberNodeId i_node_id, uint16_t i_cmd_id, const NSSPI::ByteBuffer& payload);

	/**
	 * @brief Send a ZCL unicast command
	 *
	 * @param i_node_id Short address of destination
	 * @param i_endpoint Destination endpoint
	 * @param i_cluster_id Concerned cluster
	 * @param i_cmd_id Command ID
	 * @param i_direction Message direction (client to sorver or server to client)
	 * @param i_payload Payload of the command
	 * @param i_grp_id Multicast group address to use (0 is assume as unicast/broadcast)
	 * @param i_manufacturer_code Manufacturer code
	 *
	 * @return true if message can be send
	 */
	bool SendZCLCommand(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint8_t i_cmd_id,
						const NSEZSP::EZCLFrameCtrlDirection i_direction, const NSSPI::ByteBuffer& i_payload,
						const uint16_t i_node_id, const uint8_t i_transaction_number = 0,
						const uint16_t i_grp_id = 0, const uint16_t i_manufacturer_code = 0xFFFF);

	/**
	 * @brief Send a Read Attribute command
	 *
	 * @param i_endpoint Destination endpoint
	 * @param i_cluster_id Concerned cluster
	 * @param i_attribute_id Attribute id to read
	 * @param i_direction Message direction (client to server or server to client)
	 * @param i_node_id Short address of destination
	 * @param i_grp_id Multicast group address to use (0 is assume as unicast/broadcast)
	 * @param i_manufacturer_code Manufacturer code
	 */
	bool ReadAttribute(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint16_t i_attribute_id,
					   const EZCLFrameCtrlDirection i_direction, const uint16_t i_node_id,
					   const uint8_t i_transaction_number = 0, const uint16_t i_grp_id = 0, const uint16_t i_manufacturer_code = 0xFFFF);
	
	/**
	 * @brief Send a WriteAttribute command
	 *
	 * @param i_endpoint Destination endpoint
	 * @param i_cluster_id Concerned cluster
	 * @param i_attribute_id Attribute to write
	 * @param i_direction Message direction (client to server or server to client)
	 * @param i_datatype Type of data to be written
	 * @param i_data Data to write
	 * @param i_node_id Short address of destination
	 * @param i_grp_id Multicast group address to use (0 is assume as unicast/broadcast)
	 * @param i_manufacturer_code Manufacturer code
	 *
	 * @return true if message can be send
	 */
	bool WriteAttribute(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint16_t i_attribute_id,
						const EZCLFrameCtrlDirection i_direction, const uint8_t i_datatype, const NSSPI::ByteBuffer& i_data,
						const uint16_t i_node_id, const uint8_t i_transaction_number = 0,
						const uint16_t i_grp_id = 0, const uint16_t i_manufacturer_code = 0xFFFF);

	/**
	 * @brief Send a Configure Reporting command
	 *
	 * @param i_endpoint Destination endpoint
	 * @param i_cluster_id Concerned cluster
	 * @param i_attribute_id Attribute id
	 * @param i_direction Message direction (client to server or server to client)
	 * @param i_datatype Attribute type
	 * @param i_min Minimum reporting interval
	 * @param i_max Maximum reporting interval
	 * @param i_reportable Reportable change
	 * @param i_node_id Short address of destination
	 * @param i_grp_id Multicast group address to use (0 is assume as unicast/broadcast)
	 * @param i_manufacturer_code Manufacturer code
	 */
	bool ConfigureReporting(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint16_t i_attribute_id,
							const EZCLFrameCtrlDirection i_direction, const uint8_t i_datatype, const uint16_t i_min,
							const uint16_t i_max, const uint16_t i_reportable, const uint16_t i_node_id, 
							const uint8_t i_transaction_number = 0, const uint16_t i_grp_id = 0, const uint16_t i_manufacturer_code = 0xFFFF);

private:
	void setState(CLibEzspInternal::State i_new_state);
	CLibEzspInternal::State getState() const;
	void dongleInit( uint8_t ezsp_version);
	void stackInit();

	/**
	 * @brief Request the XNCP info by sending a EZSP_GET_XNCP_INFO command
	 */
	void getXncpInfo();

	/**
	 * Oberver handlers
	 */
	void handleDongleState( EDongleState i_state );
	void handleEzspRxMessage( EEzspCmd i_cmd, NSSPI::ByteBuffer i_msg_receive );
	void handleRxGpFrame( CGpFrame &i_gpf );
	void handleBootloaderPrompt();
	void handleFirmwareXModemXfr();
	void handleRxGpdId( uint32_t &i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status );

	/**
	 * @brief Handle an incoming VERSION EZSP message
	 * @param[in] i_msg_receive The incoming EZSP message
	 */
	void handleEzspRxMessage_VERSION(const NSSPI::ByteBuffer& i_msg_receive);

	/**
	 * @brief Handle an incoming EZSP_GET_XNCP_INFO EZSP message
	 * @param[in] i_msg_receive The incoming EZSP message
	 */
	void handleEzspRxMessage_EZSP_GET_XNCP_INFO(const NSSPI::ByteBuffer& i_msg_receive);

	/**
	 * @brief Handle an incoming NETWORK_STATE EZSP message
	 * @param[in] i_msg_receive The incoming EZSP message
	 */
	void handleEzspRxMessage_NETWORK_STATE(const NSSPI::ByteBuffer& i_msg_receive);

	/**
	 * @brief Handle an incoming EZSP_LAUNCH_STANDALONE_BOOTLOADER EZSP message
	 * @param[in] i_msg_receive The incoming EZSP message
	 */
	void handleEzspRxMessage_EZSP_LAUNCH_STANDALONE_BOOTLOADER(const NSSPI::ByteBuffer& i_msg_receive);

	/**
	 * @brief Handle an incoming STACK_STATUS_HANDLER EZSP message
	 * @param[in] i_msg_receive The incoming EZSP message
	 */
	void handleEzspRxMessage_STACK_STATUS_HANDLER(const NSSPI::ByteBuffer& i_msg_receive);

/* Attributes */
private:
	NSSPI::IUartDriverHandle uartHandle; /*!< A handle to the UART driver */
	const NSSPI::TimerBuilder& timerbuilder;	/*!< A builder to create timer instances */
	uint8_t exp_ezsp_min_version;   /*!< Minimum acceptable EZSP version from the EZSP adapter (should be equal or higher), at initial state then, updated with the actual version of the adapter if it is satisfactory */
	uint8_t exp_ezsp_max_version;   /*!< Maximum acceptable EZSP version from the EZSP adapter (should be equal or lower) */
	uint8_t exp_stack_type; /*!< Expected EZSP stack type from the EZSP adapter, 2=mesh */
	uint16_t xncpManufacturerId;    /*!< The XNCP manufacturer ID read from the EZSP adatper (or 0 if unknown) */
	uint16_t xncpVersionNumber;    /*!< The XNCP version number read from the EZSP adatper (or 0 if unknown) */
	std::vector<uint8_t> dongleEUI64;    /*!< The dongle EUI64 MAC address */
	CLibEzspInternal::State lib_state;    /*!< Current state for our internal state machine */
	FLibStateCallback obsStateCallback;	/*!< Optional user callback invoked by us each time library state change */
	CEzspDongle dongle; /*!< Dongle manipulation handler */
	CZigbeeMessaging zb_messaging;  /*!< Zigbee messages utility */
	CZigbeeNetworking zb_nwk;   /*!< Zigbee networking utility */
	CGpSink gp_sink;    /*!< Internal Green Power sink utility */
	FGpFrameRecvCallback obsGPFrameRecvCallback;   /*!< Optional user callback invoked by us each time a green power message is received */
	FZclFrameRecvCallback obsZclFrameRecvCallback;	 /*!< Optional user callback invoked by us each time a zcl frame is received */
	FBindingTableRecvCallback obsBindingTableRecvCallback;  /*!< Optional user callback invoked by us each time a binding table is received */
	FZdpDeviceAnnounceCallBack obsZdpDeviceAnnounceRecvCallback; /*!< Optional user callback invoked by us each time a new device join the network */
	FZdpActiveEpCallBack obsZdpActiveEpRecvCallback; /*!< Optional user callback invoked by us each time a ZDP_ACTIVE_ENDPOINT is received */
	FTrustCenterJoinHandlerCallBack obsTrustCenterJoinHandlerCallback; /*!< Optional user callback invoked by us each time a EZSP_TRUST_CENTER_JOIN_HANDLER is received */
	FZdpSimpleDescCallBack obsZdpSimpleDescRecvCallback; /*!< Optional user callback invoked by us each time a ZDP_SIMPLE_DESC is received */
	FDongleEUI64CallBack obsDongleEUI64RecvCallback; /*!< Optional user callback invoked by us each time a EZSP_GET_EUI64 is received */
	FGpSourceIdCallback obsGPSourceIdCallback;	/*!< Optional user callback invoked by us each time a green power message is received */
	FEnergyScanCallback energyScanCallback;  /*!< A user callback invoked by us each time an energy scan is finished */
	FActiveScanCallback activeScanCallback;  /*!< A user callback invoked by us each time an active scan is finished */
	FNetworkKeyCallback networkKeyCallback;	/*!< A user callback invoked by us when the network key details are retrieved */
	bool leavePreviousNetworkAtInit;	/*!< Shall we leave any previously Zigbee network joined by the adapter, at startup? */
	unsigned int resetDot154ChannelAtInit;    /*!< If non 0, this will indicate the value of the new 802.15.4 channel on which to create a network at startup */
	bool scanInProgress;    /*!< Is there a currently ongoing network scan? */
	std::map<uint8_t, int8_t> lastChannelToEnergyScan; /*!< Map containing channel to RSSI mapping for the last energy scan */
	std::map<uint8_t, std::vector<NSEZSP::ZigbeeNetworkScanResult> > lastChannelToZigbeeNetworkScan; /*!< Map containing channel to nearby zigbee network mapping for the last active scan */
};

} // namespace NSEZSP
