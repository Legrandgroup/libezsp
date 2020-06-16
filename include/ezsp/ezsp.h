/**
 * @file ezsp.h
 * 
 * @brief Facade for NSEZSP::CLibEzspMain, hiding its internals in the published headers
 */

#ifndef __EZSP_H__
#define __EZSP_H__

#include <functional>
#include <memory>
#include <vector>
#include <map>

#include <ezsp/export.h>
#include <ezsp/config.h>
#include "ezsp/enum-generator.h"
#include <ezsp/gpd.h>
#include <ezsp/zbmessage/green-power-device.h>
#include <ezsp/zbmessage/green-power-frame.h>
#include <ezsp/ezsp-adapter-version.h>
#include <spi/TimerBuilder.h>
#include <spi/IUartDriver.h>

namespace NSEZSP {

#define CLIBEZSP_STATE_LIST(XX) \
	XX(UNINITIALIZED,=1)                    /*<! Initial state, before starting. */ \
	XX(READY,)                              /*<! Library is ready to work and process new command */ \
	XX(INIT_FAILED,)                        /*<! Initialisation failed, Library is out of work */ \
	XX(SINK_BUSY,)                          /*<! Enclosed sink is busy executing commands */ \
	XX(IN_XMODEM_XFR,)                      /*<! Adapter is ready to perform a firmware upgrade via X-modem */ \

/**
 * @brief Possible  states of class CLibEzspMain as visible from the outside (these are much simpler than the real internal states defined in CLibEzspInternalState)
 *
 * @note The lines above describes all states known in order to build both an enum and enum-to-string/string-to-enum methods
 *       In this macro, XX is a placeholder for the macro to use for building.
 *       We start numbering from 1, so that 0 can be understood as value not found for enum-generator.h
 * @see enum-generator.h
 */

class LIBEXPORT CLibEzspPublic {
public:
	DECLARE_ENUM(State, CLIBEZSP_STATE_LIST);
};

typedef CLibEzspPublic::State CLibEzspState;    /* Shortcut for access to public state enum */

class CLibEzspMain;

typedef std::function<void (CLibEzspState i_state)> FLibStateCallback;  /*!< Callback type for method registerLibraryStateCallback() */
typedef std::function<void (uint32_t &i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status)> FGpSourceIdCallback;    /*!< Callback type for method registerGPSourceIdCallback() */
typedef std::function<void (CGpFrame &i_gpf)> FGpFrameRecvCallback; /*!< Callback type for method registerGPFrameRecvCallback() */
typedef std::function<void (std::map<uint8_t, int8_t>)> FEnergyScanCallback;    /*!< Callback type for method startEnergyScan() */

class LIBEXPORT CEzsp {
public:
    /**
     * @brief Default constructor with minimal args to initialize library
     *
     * @param uartHandle A handle on a IUartDriver instance to send/receive EZSP message over a serial line
     * @param timerbuilder A timer builder object used to generate timers
     * @param requestZbNetworkResetToChannel Set this to non 0 if we should destroy any pre-existing Zigbee network in the EZSP adapter and recreate a new Zigbee network on the specified 802.15.4 channel number
     */
	CEzsp(NSSPI::IUartDriverHandle uartHandle, const NSSPI::TimerBuilder& timerbuilder, unsigned int requestZbNetworkResetToChannel=0);

    /**
     * @brief Startup the EZSP adapter
     * 
     * @note Calling this method is required after instanciation and before any data is sent to/received from the EZSP adatper
     */
    void start();

	/**
	 * @brief Get the EZSP adapter's version
	 * 
	 * @return The EZSP adapter version (hardware and firmware) if already known
	 */
	NSEZSP::EzspAdapterVersion getAdapterVersion() const;

    /**
     * @brief Instruct the library to directly switch to firmware upgrade mode at init if we get an EZSP timeout
     * 
     * This allows to succeed switching to firmware upgrade mode if the adapter EZSP initialization times out (often meaning we are already in the bootloader)
     * Do not invoke this if you are not planning to do a firmware upgrade right now, as the lib will not escape from bootloader (into EZSP mode) anymore
     */
    void forceFirmwareUpgradeOnInitTimeout();

    /**
     * @brief Register callback on current library state
     *
     * @param newObsStateCallback A callback function of type void func(CLibEzspState i_state), that will be invoked each time our internal state will change (or nullptr to disable callbacks)
     */
    void registerLibraryStateCallback(FLibStateCallback newObsStateCallback);

    /**
     * @brief Register callback to receive all incoming greenpower sourceId
     *
     * @param newObsGPFrameRecvCallback A callback function of type void func(CGpFrame &i_gpf), that will be invoked each time a new valid green power frame is received from a known source ID (or nullptr to disable callbacks)
     */
    void registerGPFrameRecvCallback(FGpFrameRecvCallback newObsGPFrameRecvCallback);

    /**
     * @brief Register callback to receive all incoming greenpower sourceId
     *
     * @param newObsGPSourceIdCallback A callback function of type void func(uint32_t &i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status), that will be invoked each time a new source ID transmits over the air (or nullptr to disable callbacks)
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
     * @brief Switch the adapter to firmware upgrade mode
     * 
     * Once this is done, the adapter will wait for an X-modem transfer of a new firmware
     */
    void setFirmwareUpgradeMode();

    /**
     * @brief Start an energy scan on the EZSP adapter
     * 
     * When the scan is complete, a EZSP_ENERGY_SCAN_RESULT_HANDLER EZSP message will be received from the adapter
     * 
     * @param duration The exponent of the number of scan periods, where a scan period is 960 symbols. The scan will occur for ((2^duration) + 1) scan periods((2^duration) + 1) scan periods
     *                 The default value (3) allows for a quite fast scan. Values above 6 may result in longer scan duration.
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
    NSSPI::GenericAsyncDataInputObservable* getAdapterSerialReadObservable() const;

private:
	CLibEzspMain *main;
};
} //namespace NSEZSP 

#endif
