/**
 * @file lib-ezsp-main.h
 *
 * @brief Main entry point for drive lib ezsp
 */
 
#pragma once

#include "spi/IUartDriver.h"
#include "spi/TimerBuilder.h"

#include "ezsp/ezsp-dongle.h"
#include "ezsp/zigbee-tools/zigbee-networking.h"
#include "ezsp/zigbee-tools/zigbee-messaging.h"
#include "ezsp/zigbee-tools/green-power-sink.h"
#include "ezsp/zbmessage/green-power-device.h"

#include "ezsp/ezsp-dongle-observer.h"
#include "ezsp/green-power-observer.h"


/**
 * @brief Possible states of the state machine used by class CLibEzspMain
 */
enum class CLibEzspState { 
    NO_INIT,            /*<! Initial state, before starting. */
    READY,              /*<! Library is ready to work and process new command */
    INIT_FAILED,        /*<! Initialisation failed, Library is out of work */
    INIT_IN_PROGRESS,   /*<! Initialisation in progress, no other command can be process */
};

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
     * @param i_timer_factory An ITimerFactory used to generate ITimer objects
     */
    CLibEzspMain( IUartDriver* uartDriver, TimerBuilder &i_timer_factory );

    CLibEzspMain() = delete; /*<! Construction without arguments is not allowed */
    CLibEzspMain(const CLibEzspMain&) = delete; /*<! No copy construction allowed */
    CLibEzspMain& operator=(CLibEzspMain) = delete; /*<! No assignment allowed */    

    CGpSink& getSink() { return this->gp_sink; }

    /**
     * @brief Register callback on current library state
     *
     * @param newObsStateCallback A callback function of type void func(CLibEzspState& i_state), that will be invoked each time our internal state will change (or nullptr to disable callbacks)
     */
    void registerLibraryStateCallback(std::function<void (CLibEzspState& i_state)> newObsStateCallback);

    /**
     * @brief Register callback to receive all incomming greenpower sourceId
     *
     * @param newObsGPSourceIdCallback A callback function of type void func(uint32_t &i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status), that will be invoked each time a new source ID transmits over the air (or nullptr to disable callbacks)
     */
    void registerGPSourceIdCallback(std::function<void (uint32_t &i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status)> newObsGPSourceIdCallback);


private:
    uint8_t exp_ezsp_version;   /*!< Expected EZSP version from dongle, at initial state then current version of dongle */
    CLibEzspState lib_state;    /*!< Current state for our internal state machine */
    std::function<void (CLibEzspState& i_state)> obsStateCallback;	/*!< Optional user callback invoked by us each time library state change */
    std::unique_ptr<ITimer> timer;  /*!< Internal timer */
    CEzspDongle dongle; /*!< Dongle manipulation handler */
    CZigbeeMessaging zb_messaging;  /*!< Zigbee messages utility */
    CZigbeeNetworking zb_nwk;   /*!< Zigbee networking utility */
    CGpSink gp_sink;    /*!< Internal Green Power sink utility */
    std::function<void (uint32_t &i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status)> obsGPSourceIdCallback;	/*!< Optional user callback invoked by us each time a green power message is received */

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
