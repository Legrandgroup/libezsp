/**
 * 
 * 
 */
#pragma once

#include "../domain/ezsp-dongle.h"
#include "../domain/zigbee-tools/zigbee-networking.h"
#include "../domain/zigbee-tools/zigbee-messaging.h"
#include "../spi/IUartDriver.h"

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
    CAppDemo(IUartDriver *uartDriver);

    /**
     * Callback
     */
    void handleDongleState( EDongleState i_state );
    void handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive );

private:
    CEzspDongle dongle;
    CZigbeeNetworking zb_nwk;
    CZigbeeMessaging zb_messaging;
    EAppState app_state;

    void setAppState( EAppState i_state );
    void dongleInit();
    void stackInit();
    
};

