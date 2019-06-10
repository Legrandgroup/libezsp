/**
 * @file zigbee-networking.h
 */
#pragma once

#include "zigbee-messaging.h"

#include "../ezsp-dongle-observer.h"
#include "../ezsp-dongle.h"
#include "../zbmessage/zigbee-message.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

#define DEFAULT_RADIO_CHANNEL 11

class CZigbeeNetworking : public CEzspDongleObserver
{
public:
    CZigbeeNetworking( CEzspDongle &i_dongle, CZigbeeMessaging &i_zb_messaging );

    CZigbeeNetworking() = delete; /* Construction without arguments is not allowed */
    CZigbeeNetworking(const CZigbeeNetworking&) = delete; /* No copy construction allowed */

    CZigbeeNetworking& operator=(CZigbeeNetworking) = delete; /* No assignment allowed */

    void stackInit(SEzspConfig *l_config, uint8_t l_config_size, SEzspPolicy *l_policy, uint8_t l_policy_size);

    void formHaNetwork(uint8_t channel=DEFAULT_RADIO_CHANNEL);
    void OpenNetwork( uint8_t i_timeout );
    void CloseNetwork( void );
    void LeaveNetwork( void );
    
    void startDiscoverProduct(std::function<void (EmberNodeType i_type, EmberEUI64 i_eui64, EmberNodeId i_id)> i_discoverCallbackFct = nullptr);

    // Green Power
    

    /**
     * Observer
     */
    void handleDongleState( EDongleState i_state ){;}
    void handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive );    

private:
    CEzspDongle &dongle;
    CZigbeeMessaging &zb_messaging;
    uint8_t child_idx;
    std::function<void (EmberNodeType i_type, EmberEUI64 i_eui64, EmberNodeId i_id)> discoverCallbackFct;
    uint8_t form_channel; // radio channel to form network
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
