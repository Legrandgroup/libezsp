/**
 * @file CAppDemo.h
 */

#pragma once

#include "../domain/ezsp-dongle.h"
#include "../domain/zigbee-tools/zigbee-networking.h"
#include "../domain/zigbee-tools/zigbee-messaging.h"
#include "../spi/IUartDriver.h"
#include "../spi/ITimerFactory.h"
#include "../spi/ILogger.h"
#include "dummy_db.h"

typedef enum
{
    APP_NOT_INIT, // starting state
    APP_READY,  // default state if no action in progress
    APP_ERROR,  // something wrong
    APP_INIT_IN_PROGRESS, // dongle init in progress
    APP_FORM_NWK_IN_PROGRESS // form a network in progress
}EAppState;

class CAppDemo : public CEzspDongleObserver
{
public:
    CAppDemo(IUartDriver *uartDriver, ITimerFactory& i_timer_factory);

    /**
     * Callback
     */
    void handleDongleState( EDongleState i_state );
    void handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive );

private:
    void setAppState( EAppState i_state );
    void dongleInit();
    void stackInit();

private:
    CEzspDongle dongle;
    CZigbeeMessaging zb_messaging;
    CZigbeeNetworking zb_nwk;
    EAppState app_state;
    Cdb db;
};

