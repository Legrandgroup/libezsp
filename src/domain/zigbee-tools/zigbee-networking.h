/**
 * 
 */
#pragma once

#include "zigbee-messaging.h"

#include "../ezsp-dongle-observer.h"
#include "../ezsp-dongle.h"
#include "../zbmessage/zigbee-message.h"

/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>

class CZigbeeNetworking : public CEzspDongleObserver
{
public:
    CZigbeeNetworking( CEzspDongle &i_dongle, CZigbeeMessaging &i_zb_messaging );

    void stackInit(SEzspConfig *l_config, uint8_t l_config_size, SEzspPolicy *l_policy, uint8_t l_policy_size);

    void formHaNetwork();
    void OpenNetwork( uint8_t i_timeout );
    void CloseNetwork( void );

    void startDiscoverProduct(std::function<void (EmberNodeType i_type, EmberEUI64 i_eui64, EmberNodeId i_id)> i_discoverCallbackFct = nullptr);

    /**
     * Observer
     */
    void handleDongleState( EDongleState /* i_state */ ) {;}
    void handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive );

private:
    CEzspDongle &dongle;
    CZigbeeMessaging &zb_messaging;
    uint8_t child_idx;
    std::function<void (EmberNodeType i_type, EmberEUI64 i_eui64, EmberNodeId i_id)> discoverCallbackFct;
};

#include <pp/official_api_end.h>
