/**
 * @file zigbee-messaging.h
 *
 * @brief Manages zigbee message, timeout, retry
 */
#pragma once

#include "ezsp/ezsp-dongle-observer.h"
#include "ezsp/ezsp-dongle.h"
#include "ezsp/zbmessage/zigbee-message.h"
#include "spi/ByteBuffer.h"

namespace NSEZSP {

class CZigbeeMessaging : public CEzspDongleObserver
{
public:
    CZigbeeMessaging( CEzspDongle &i_dongle, NSSPI::TimerBuilder &i_timer_factory );

    void SendBroadcast( EOutBroadcastDestination i_destination, uint8_t i_radius, CZigBeeMsg i_msg);
    void SendUnicast( EmberNodeId i_node_id, CZigBeeMsg i_msg );

    /**
     * @brief Send a ZDO unicast command
     * @param i_node_id Short address of destination
     * @param i_cmd_id Command
     * @param[in] payload Payload for the ZDO unicast
     * @return true if message can be send
     */
    void SendZDOCommand( EmberNodeId i_node_id, uint16_t i_cmd_id, const NSSPI::ByteBuffer& payload );

    /**
     * Observer
     */
    void handleEzspRxMessage( EEzspCmd i_cmd, NSSPI::ByteBuffer i_msg_receive );

private:
    CEzspDongle &dongle;
    NSSPI::TimerBuilder &timer_factory; // needed in the future to well manage retry/timeout on unicast zigbee message
};

} // namespace NSEZSP
