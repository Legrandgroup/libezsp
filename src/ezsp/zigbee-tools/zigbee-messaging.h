/**
 * @file zigbee-messaging.h
 *
 * @brief Manages zigbee message, timeout, retry
 */
#pragma once

#include <vector>

#include "ezsp/ezsp-dongle-observer.h"
#include "ezsp/ezsp-dongle.h"
#include "ezsp/zbmessage/zigbee-message.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

class CZigbeeMessaging : public CEzspDongleObserver
{
public:
    CZigbeeMessaging( CEzspDongle &i_dongle, ITimerFactory &i_timer_factory );

    void SendBroadcast( EOutBroadcastDestination i_destination, uint8_t i_radius, CZigBeeMsg i_msg);
    void SendUnicast( EmberNodeId i_node_id, CZigBeeMsg i_msg );

    /**
     * @brief SendSpecificCommand : Permit to send a ZDO unicast command
     * @param i_node_id     : short address of destination
     * @param i_cmd_id      : command
     * @param payload       : payload of command
     * @return true if message can be send
     */
    void SendZDOCommand( EmberNodeId i_node_id, uint16_t i_cmd_id, std::vector<uint8_t> payload );

    /**
     * Observer
     */
    void handleDongleState( EDongleState i_state ){(void) i_state;}
    void handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive );

private:
    CEzspDongle &dongle;
    ITimerFactory &timer_factory; // needed in the future to well manage retry/timeout on unicast zigbee message
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
