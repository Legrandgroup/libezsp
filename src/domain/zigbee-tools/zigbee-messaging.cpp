/**
 * 
 */

#include "zigbee-messaging.h"


CZigbeeMessaging::CZigbeeMessaging( CEzspDongle &i_dongle ): dongle(i_dongle)
{
    dongle.registerObserver(this);
}

void CZigbeeMessaging::handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive )
{
    switch( i_cmd )
    {
        case EZSP_MESSAGE_SENT_HANDLER:
        {
            std::cout << "EZSP_MESSAGE_SENT_HANDLER return status : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(16))) << std::endl;
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
    std::vector<uint8_t> l_payload;
    std::vector<uint8_t> l_zb_msg = i_msg.Get();

    // destination
    l_payload.push_back( static_cast<uint8_t>(i_destination&0xFF) );
    l_payload.push_back( static_cast<uint8_t>((i_destination>>8)&0xFF) );

    // aps frame
    std::vector<uint8_t> v_tmp = i_msg.GetAps()->GetEmberAPS();
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
