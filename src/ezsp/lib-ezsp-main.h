/**
 * @file lib-ezsp-main.h
 *
 * @brief Main entry point for drive lib ezsp
 */

#pragma once

#include "spi/IUartDriver.h"
#include "spi/TimerBuilder.h"

#include <ezsp/ezsp.h>
#include "ezsp/lib-ezsp-main.h"
#include "ezsp/ezsp-dongle.h"
#include "ezsp/zigbee-tools/zigbee-networking.h"
#include "ezsp/zigbee-tools/zigbee-messaging.h"
#include "ezsp/zigbee-tools/green-power-sink.h"
#include <ezsp/zbmessage/green-power-device.h>

#include "ezsp/ezsp-dongle-observer.h"
#include "ezsp/green-power-observer.h"


namespace NSEZSP {
/**
 * @brief Class allowing sending commands and receiving events from an EZSP interface
 */
class CLibEzspMain : public CEzspDongleObserver, CGpObserver
{
public:
    /**
     * @brief Default constructor with minimal args to initialize library
     *
     * @param uartDriver An IUartDriver instance to send/receive EZSP message over a serial line
     * @param timerbuilder An ITimerFactory used to generate ITimer objects
     */
    CLibEzspMain( NSSPI::IUartDriver* uartDriver, NSSPI::TimerBuilder &timerbuilder );

    CLibEzspMain() = delete; /*<! Construction without arguments is not allowed */
    CLibEzspMain(const CLibEzspMain&) = delete; /*<! No copy construction allowed */
    CLibEzspMain& operator=(CLibEzspMain) = delete; /*<! No assignment allowed */

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
    void registerGPSourceIdCallback(FGpdSourceIdCallback newObsGPSourceIdCallback);

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
    bool removeGPDevices(std::vector<uint32_t>& sourceIdList);

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
     * @brief Controls the answer to request channel messages sent by GP devices
     *
     * @param allowed Set to true if answers to request channel is allowed
     */
    void setAnswerToGpfChannelRqstPolicy(bool allowed);

    /**
     * @brief Switch the embedded firmware to booloader prompt mode
     */
    void jumpToBootloader();

private:
    NSSPI::TimerBuilder &timerbuilder;	/*!< A builder to create timer instances */
    uint8_t exp_ezsp_version;   /*!< Expected EZSP version from dongle, at initial state then current version of dongle */
    CLibEzspState lib_state;    /*!< Current state for our internal state machine */
    FLibStateCallback obsStateCallback;	/*!< Optional user callback invoked by us each time library state change */
    CEzspDongle dongle; /*!< Dongle manipulation handler */
    CZigbeeMessaging zb_messaging;  /*!< Zigbee messages utility */
    CZigbeeNetworking zb_nwk;   /*!< Zigbee networking utility */
	FGpFrameRecvCallback obsGPFrameRecvCallback;   /*!< Optional user callback invoked by us each time a green power message is received */
    CGpSink gp_sink;    /*!< Internal Green Power sink utility */
    FGpdSourceIdCallback obsGPSourceIdCallback;	/*!< Optional user callback invoked by us each time a green power message is received */

    void setState( CLibEzspState i_new_state );
    CLibEzspState getState() const;
    void dongleInit( uint8_t ezsp_version);
    void stackInit();

    /**
     * Oberver handlers
     */
    void handleDongleState( EDongleState i_state );
    void handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive );
    void handleRxGpFrame( CGpFrame &i_gpf );
    void handleRxGpdId( uint32_t &i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status );
};

} // namespace NSEZSP
