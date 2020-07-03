/**
 * @file lib-ezsp-main.h
 *
 * @brief Main API methods for libezsp
 */

#pragma once

#include <map>
#include <ctime>	// For std::time_t
#include <fstream>

#include "spi/IUartDriver.h"
#include "spi/TimerBuilder.h"

#include <ezsp/ezsp.h>
#include "ezsp/enum-generator.h"
#include "ezsp/ezsp-dongle.h"
#include "ezsp/zigbee-tools/zigbee-networking.h"
#include "ezsp/zigbee-tools/zigbee-messaging.h"
#include "ezsp/zigbee-tools/green-power-sink.h"
#include "ezsp/zbmessage/green-power-device.h"
#include "spi/ByteBuffer.h"

#include "ezsp/ezsp-dongle-observer.h"
#include "ezsp/green-power-observer.h"


namespace NSEZSP {

namespace Stats {
	class SourceIdData {
public:
		static constexpr std::time_t unknown = std::time_t(-1);
		static constexpr unsigned int REPORTS_AVG_PERIOD = 130;	/* 130s in average between two reports */

		SourceIdData() : lastSeenTimeStamp(unknown), offlineSequenceNo(0), nbSuccessiveMisses(0), nbSuccessiveRx(0), outputFile() {}

		std::time_t lastSeenTimeStamp;
		uint16_t offlineSequenceNo;
		uint16_t nbSuccessiveMisses;
		uint32_t nbSuccessiveRx;
		std::fstream outputFile;
	};
}
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
	 * @param requestZbNetworkResetToChannel Set this to non 0 if we should destroy any pre-existing Zigbee network in the EZSP adapter and recreate a new Zigbee network on the specified 802.15.4 channel number
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
	 * @param newObsEnergyScanCallback A callback function of type void func(std::map<uint8_t, int8_t>)> that will be invoked when the energy scan is finished
	 *                                 The map provided to the callback contains entries with the key (uint8_t) being the 802.15.4 channel, and the value (int8_t) being the measured RSSI on this channel
	 * @param duration The exponent of the number of scan periods, where a scan period is 960 symbols. The scan will occur for ((2^duration) + 1) scan periods((2^duration) + 1) scan periods
	 *
	 * @return true If the scan could be started, false otherwise (adapter is not ready, maybe a scan is already ongoing)
	 */
	bool startEnergyScan(FEnergyScanCallback energyScanCallback, uint8_t duration = 3);

	/**
	 * @brief Select the 802.15.4 channel on which the EZSP adapter works
	 *
	 * @param channel The 802.15.4 channel (valid values are 11 to 26, inclusive)
	 *
	 * @return true If the channel could be set
	 */
	bool setChannel(uint8_t channel);

private:
	NSSPI::IUartDriverHandle uartHandle; /*!< A handle to the UART driver */
	const NSSPI::TimerBuilder& timerbuilder;	/*!< A builder to create timer instances */
	uint8_t exp_ezsp_min_version;   /*!< Minimum acceptable EZSP version from the EZSP adapter (should be equal or higher), at initial state then, updated with the actual version of the adapter if it is satisfactory */
	uint8_t exp_ezsp_max_version;   /*!< Maximum acceptable EZSP version from the EZSP adapter (should be equal or lower) */
	uint8_t exp_stack_type; /*!< Expected EZSP stack type from the EZSP adapter, 2=mesh */
	uint16_t xncpManufacturerId;    /*!< The XNCP manufacturer ID read from the EZSP adatper (or 0 if unknown) */
	uint16_t xncpVersionNumber;    /*!< The XNCP version number read from the EZSP adatper (or 0 if unknown) */
	CLibEzspInternal::State lib_state;    /*!< Current state for our internal state machine */
	FLibStateCallback obsStateCallback;	/*!< Optional user callback invoked by us each time library state change */
	CEzspDongle dongle; /*!< Dongle manipulation handler */
	CZigbeeMessaging zb_messaging;  /*!< Zigbee messages utility */
	CZigbeeNetworking zb_nwk;   /*!< Zigbee networking utility */
	CGpSink gp_sink;    /*!< Internal Green Power sink utility */
	FGpFrameRecvCallback obsGPFrameRecvCallback;   /*!< Optional user callback invoked by us each time a green power message is received */
	FGpSourceIdCallback obsGPSourceIdCallback;	/*!< Optional user callback invoked by us each time a green power message is received */
	FEnergyScanCallback obsEnergyScanCallback;  /*!< Optional user callback invoked by us each time an energy scan is finished */
	unsigned int resetDot154ChannelAtInit;    /*!< Do we destroy any pre-existing Zigbee network in the adapter at startup (!=0), if so this will contain the value of the new 802.15.4 channel to use */
	bool scanInProgress;    /*!< Is there a currently ongoing network scan? */
	std::map<uint8_t, int8_t> lastChannelToEnergyScan; /*!< Map containing channel to RSSI mapping for the last energy scan */
	std::map<uint32_t, NSEZSP::Stats::SourceIdData> registeredSourceIdsStats; /* For Anthony's testing, the current statistics per source ID */

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
	void handleBootloaderPrompt();
	void handleFirmwareXModemXfr();
	void handleRxGpFrame( CGpFrame &i_gpf );
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
};

} // namespace NSEZSP
