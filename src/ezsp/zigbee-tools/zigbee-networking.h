/**
 * @file zigbee-networking.h
 */
#pragma once

#include "ezsp/zigbee-tools/zigbee-messaging.h"

#include "ezsp/ezsp-dongle-observer.h"
#include "ezsp/ezsp-dongle.h"
#include "ezsp/zbmessage/zigbee-message.h"

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

    void stackInit(const std::vector<SEzspConfig>& l_config, const std::vector<SEzspPolicy>& l_policy);

    void formHaNetwork(uint8_t channel=DEFAULT_RADIO_CHANNEL);

    /**
     * @brief Open the zigbee network for devices to join
     *
     * @param i_timeout A timeout for the network to open (in seconds)
     */
    void openNetwork(uint8_t i_timeout);

    /**
     * @brief Close the zigbee network
     *
     * @param i_timeout A timeout for the network to open (in seconds)
     */
    void closeNetwork();

    /**
     * @brief Make the adapter leave the current zigbee network it has joined (if any)
     */
    void leaveNetwork();

    void startDiscoverProduct(std::function<void (EmberNodeType i_type, EmberEUI64 i_eui64, EmberNodeId i_id)> i_discoverCallbackFct = nullptr);

    // Green Power


    /**
     * Observer
     */
    void handleDongleState( EDongleState /* i_state */ ){;}
    void handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive );
    void handleBootloaderPrompt(){;}

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
