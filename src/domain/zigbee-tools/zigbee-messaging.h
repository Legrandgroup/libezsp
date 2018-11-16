/**
 * Used to managed zigbee message, timeout, retry, ... TODO
 */
#pragma once

#include "../ezsp-dongle-observer.h"
#include "../ezsp-dongle.h"
#include "../zbmessage/zigbee-message.h"

class CZigbeeMessaging : public CEzspDongleObserver
{
public:
    CZigbeeMessaging( CEzspDongle &i_dongle );

    void SendBroadcast( EOutBroadcastDestination i_destination, uint8_t i_radius, CZigBeeMsg i_msg);
    
    /**
     * Observer
     */
    void handleDongleState( EDongleState i_state ){;}
    void handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive );    

private:
    CEzspDongle &dongle;
};
