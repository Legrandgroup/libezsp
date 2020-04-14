/**
 * @file zigbee-messaging.cpp
 *
 * @brief Manages zigbee message, timeout, retry
 */

#include "ezsp/zigbee-tools/zigbee-messaging.h"

#include "spi/ILogger.h"

using NSEZSP::CZigbeeMessaging;

CZigbeeMessaging::CZigbeeMessaging(CEzspDongle& i_dongle, const NSSPI::TimerBuilder& i_timer_builder) :
	dongle(i_dongle),
	timerBuilder(i_timer_builder)
{
    dongle.registerObserver(this);
}

void CZigbeeMessaging::handleEzspRxMessage( EEzspCmd i_cmd, NSSPI::ByteBuffer i_msg_receive )
{
    switch( i_cmd )
    {
        case EZSP_MESSAGE_SENT_HANDLER:
        {
            clogD << "EZSP_MESSAGE_SENT_HANDLER return status : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(16))) << std::endl;
        }
        break;


        default:
        break;
    }

}

/**
 * @brief SendBroadcast : send broadcast zigbee message
 * @param i_destination : type of node concern by broadcast
 * @param radius : The message will be delivered to all nodes within radius hops of the sender.
 *                  A radius of zero is converted to EMBER_MAX_HOPS.
 * @param i_msg : meassge to send
 */
void CZigbeeMessaging::SendBroadcast( EOutBroadcastDestination i_destination, uint8_t i_radius, CZigBeeMsg i_msg)
{
    NSSPI::ByteBuffer l_payload;
    NSSPI::ByteBuffer l_zb_msg = i_msg.Get();

    // destination
    l_payload.push_back( static_cast<uint8_t>(i_destination&0xFF) );
    l_payload.push_back( static_cast<uint8_t>(static_cast<uint8_t>(i_destination>>8)&0xFF) );

    // aps frame
    NSSPI::ByteBuffer v_tmp = i_msg.GetAps().GetEmberAPS();
    l_payload.insert(l_payload.end(), v_tmp.begin(), v_tmp.end());

    // radius
    l_payload.push_back( i_radius );

    // message tag : not used for this simplier demo
    l_payload.push_back( 0 );

    // message length
    l_payload.push_back( static_cast<uint8_t>(l_zb_msg.size()) );

    // message content
    l_payload.insert(l_payload.end(), l_zb_msg.begin(), l_zb_msg.end());


    dongle.sendCommand(EZSP_SEND_BROADCAST, l_payload);
}

/**
 * @brief SendUnicast : send direct unicast zigbee message
 * @param i_node_id : destination short address
 * @param i_msg : meassge to send
 */
void CZigbeeMessaging::SendUnicast( EmberNodeId i_node_id, CZigBeeMsg i_msg )
{
    NSSPI::ByteBuffer l_payload;
    NSSPI::ByteBuffer l_zb_msg = i_msg.Get();

    // only direct unicast is supported for now
    l_payload.push_back( EMBER_OUTGOING_DIRECT );

    // destination
    l_payload.push_back( static_cast<uint8_t>(i_node_id&0xFF) );
    l_payload.push_back( static_cast<uint8_t>(static_cast<uint8_t>(i_node_id>>8)&0xFF) );

    // aps frame
    NSSPI::ByteBuffer v_tmp = i_msg.GetAps().GetEmberAPS();
    l_payload.insert(l_payload.end(), v_tmp.begin(), v_tmp.end());

    // message tag : not used for this simplier demo
    l_payload.push_back( 0 );

    // message length
    l_payload.push_back( static_cast<uint8_t>(l_zb_msg.size()) );

    // message content
    l_payload.insert(l_payload.end(), l_zb_msg.begin(), l_zb_msg.end());


    dongle.sendCommand(EZSP_SEND_UNICAST, l_payload);
}

/**
 * @brief SendSpecificCommand : Permit to send a ZDO unicast command
 * @param i_node_id     : short address of destination
 * @param i_cmd_id      : command
 * @param payload       : payload of command
 * @return true if message can be send
 */
void CZigbeeMessaging::SendZDOCommand(EmberNodeId i_node_id, uint16_t i_cmd_id, const NSSPI::ByteBuffer& payload)
{
    CZigBeeMsg l_msg;

    l_msg.SetZdo( i_cmd_id, payload, 0/*network.GetNextTransactionNb(i_node_id, 0)*/ );

    SendUnicast( i_node_id, l_msg );
}
