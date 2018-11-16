/**
 * 
 */
#pragma once

#include "zigbee-messaging.h"

#include "../ezsp-dongle-observer.h"
#include "../ezsp-dongle.h"
#include "../zbmessage/zigbee-message.h"

class CZigbeeNetworking : public CEzspDongleObserver
{
public:
    CZigbeeNetworking( CEzspDongle &i_dongle, CZigbeeMessaging &i_zb_messaging );

    void formHaNetwork();
    void OpenNetwork( uint8_t i_timeout );
    void CloseNetwork( void );
    
    void startDiscoverProduct();

    /**
     * Observer
     */
    void handleDongleState( EDongleState i_state ){;}
    void handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive );    

private:
    CEzspDongle &dongle;
    CZigbeeMessaging &zb_messaging;
    uint8_t child_idx;
};
