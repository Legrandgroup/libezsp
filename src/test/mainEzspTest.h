/**
 * 
 * 
 */
#pragma once

#include "../domain/ezsp-dongle.h"
#include "../spi/IUartDriver.h"

typedef enum
{
    APP_NOT_INIT, // starting state
    APP_READY,  // default state if no action in progress
    APP_ERROR,  // something wrong
    APP_INIT_IN_PROGRESS, // dongle init in progress
    APP_FORM_NWK_IN_PROGRESS // form a network in progress
}EAppState;

class CAppDemo : public CDongleHandler
{
public:
    CAppDemo(IUartDriver *uartDriver);

    /**
     * Callback
     */
    void dongleState( EDongleState i_state );
    void ashRxMessage( std::vector<uint8_t> i_message );
    void ezspHandler( EEzspCmd i_cmd, std::vector<uint8_t> i_message );

private:
    CEzspDongle dongle;
    EAppState app_state;
    uint8_t child_idx;

    void setAppState( EAppState i_state );
    void dongleInit();
    void stackInit();
    void formHaNetwork();
    bool OpenNetwork( uint8_t i_timeout );
    bool CloseNetwork( void );
    void SendBroadcast( EOutBroadcastDestination i_destination, uint8_t i_radius, CZigBeeMsg i_msg);
    void startDiscoverProduct();
};

