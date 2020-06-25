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

class CZigbeeMessaging : public CEzspDongleObserver {
public:
	/**
	 * @brief Constructor
	 *
	 * @param[in] i_dongle The EZSP adapter used to send/receive Zigbee messages
	 * @param[in] i_timer_builder Timer builder object used to generate timers
	 */
	CZigbeeMessaging(CEzspDongle& i_dongle, const NSSPI::TimerBuilder& i_timer_builder);

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
	const NSSPI::TimerBuilder& timerBuilder; // needed in the future to well manage retry/timeout on unicast zigbee message
};

} // namespace NSEZSP
