/**
 * 
 */

#include "out-zigbee-message.h"

#include "ezsp-enum.h"


void COutZbMessage::SetState( EOutMsgState i_state )
{
  state = i_state;
  if( i_state == E_ZB_MSG_WAITING_EZSP_ACK ){ timeout_timer = 1000; }
  else if( i_state == E_ZB_MSG_WAITING_ZB_ACK ){ timeout_timer = 10000; }
  else{ timeout_timer = 1000; }
}

/**
 * @brief DecTimeout : decrement timeout value
 * @param i_timeout : value of decrement in ms
 */
void COutZbMessage::DecTimeout( uint16_t i_dec_value )
{
  if( timeout_timer >  i_dec_value ){ timeout_timer -= i_dec_value; }
  else { timeout_timer = 0; }
}

/**
 * @brief SetDirectUnicastMessage : prepare to send a direct unicast message
 * @param i_dest_node_id : destination
 * @param i_msg_tag : user tag
 * @param i_msg : zigbee message
 */
void COutZbMessage::SetDirectUnicastMessage( uint16_t i_dest_node_id, CZigBeeMsg i_msg )
{
  msg_type = E_OUT_MSG_TYPE_DIRECT_UNICAST;

  unicast_dest_node_id = i_dest_node_id;

  zb_msg = i_msg;

  state = E_ZB_MSG_READY_TO_SEND;
  timeout_timer = 0;
}

/**
 * @brief SetBroadcastMessage : prepare to sending a broadcast message
 * @param i_destination : type of node concern by broadcast
 * @param radius : The message will be delivered to all nodes within radius hops of the sender.
 *                  A radius of zero is converted to EMBER_MAX_HOPS.
 * @param i_msg : zigbee message
 */
void COutZbMessage::SetBroadcastMessage( EOutBroadcastDestination i_destination, uint8_t i_radius, CZigBeeMsg i_msg )
{
  msg_type = E_OUT_MSG_TYPE_BROADCAST;

  broadcast_dest = i_destination;
  broadcast_radius = i_radius;

  zb_msg = i_msg;

  state = E_ZB_MSG_READY_TO_SEND;
  timeout_timer = 0;
}

/**
 * @brief GetEZSPMessage : concatenate message for EZSP protocol
 * @return : return a well form buffer payload with EZSP command
 */
std::vector<uint8_t> COutZbMessage::GetEZSPMessage(void)
{
  std::vector<uint8_t> lo_msg;
  std::vector<uint8_t> l_zb_msg = zb_msg.Get();

  switch( msg_type )
  {
    case E_OUT_MSG_TYPE_DIRECT_UNICAST:
    {
      /** \todo manage indirect unicast after ...*/
      lo_msg.push_back( EZSP_SEND_UNICAST );
      lo_msg.push_back( EMBER_OUTGOING_DIRECT );
      lo_msg.push_back( unicast_dest_node_id&0xFF );
      lo_msg.push_back( (unicast_dest_node_id>>8)&0xFF );
      std::vector<uint8_t> v_tmp = zb_msg.GetAps()->GetEmberAPS();
      lo_msg.insert(lo_msg.end(), v_tmp.begin(), v_tmp.end());
    }
    break;

    case E_OUT_MSG_TYPE_BROADCAST:
    {
      lo_msg.push_back( EZSP_SEND_BROADCAST );
      lo_msg.push_back( broadcast_dest&0xFF );
      lo_msg.push_back( broadcast_dest>>8 );
      std::vector<uint8_t> v_tmp = zb_msg.GetAps()->GetEmberAPS();
      lo_msg.insert(lo_msg.end(), v_tmp.begin(), v_tmp.end());
      lo_msg.push_back( broadcast_radius );
    }
    break;

    case E_OUT_MSG_TYPE_MULTICAST:
    {
      lo_msg.push_back( EZSP_SEND_MULTICAST );
      std::vector<uint8_t> v_tmp = zb_msg.GetAps()->GetEmberAPS();
      lo_msg.insert(lo_msg.end(), v_tmp.begin(), v_tmp.end());
      lo_msg.push_back( multicast_hops );
      lo_msg.push_back( multicast_non_member_radius );
    }
    break;
  }

  lo_msg.push_back( msg_tag );
  lo_msg.push_back( l_zb_msg.size() );
  lo_msg.insert(lo_msg.end(), l_zb_msg.begin(), l_zb_msg.end());

  return lo_msg;
}

/**
 * @brief SetEZSPMessage
 * @param i_msg
 */
void COutZbMessage::SetEZSPMessage(std::vector<uint8_t> i_msg )
{
  switch( i_msg.at(2) )
  {
    case EZSP_SEND_UNICAST:
    {
      // be sure it's a direct unicast, othe type is not manage by this way
      if( EMBER_OUTGOING_DIRECT == i_msg.at(3) )
      {
        msg_type = E_OUT_MSG_TYPE_DIRECT_UNICAST;

        unicast_dest_node_id = i_msg.at(4) & (i_msg.at(5)<<8);

        msg_tag = i_msg.at(17);

        std::vector<uint8_t> v1;
        std::vector<uint8_t> v2;
        for(uint16_t loop=6; loop < i_msg.size(); loop++)
        {
            v1.push_back(i_msg.at(loop));
        }
        for(uint16_t loop=19; loop < i_msg.size(); loop++)
        {
            v2.push_back(i_msg.at(loop));
        }

        zb_msg.Set( v1, v2 );
      }
    }
    break;

    case EZSP_SEND_BROADCAST:
    {
      msg_type = E_OUT_MSG_TYPE_BROADCAST;

      broadcast_dest = (EOutBroadcastDestination)(i_msg.at(3) & (i_msg.at(4)<<8));

      broadcast_radius = i_msg.at(16);

      msg_tag = i_msg.at(17);

        std::vector<uint8_t> v1;
        std::vector<uint8_t> v2;
        for(uint16_t loop=5; loop < i_msg.size(); loop++)
        {
            v1.push_back(i_msg.at(loop));
        }
        for(uint16_t loop=19; loop < i_msg.size(); loop++)
        {
            v2.push_back(i_msg.at(loop));
        }

        zb_msg.Set( v1, v2 );
    }
    break;

    case EZSP_SEND_MULTICAST:
    {
      msg_type = E_OUT_MSG_TYPE_MULTICAST;

      multicast_hops = i_msg.at(14);
      multicast_non_member_radius = i_msg.at(15);

      msg_tag = i_msg.at(16);

        std::vector<uint8_t> v1;
        std::vector<uint8_t> v2;
        for(uint16_t loop=3; loop < i_msg.size(); loop++)
        {
            v1.push_back(i_msg.at(loop));
        }
        for(uint16_t loop=18; loop < i_msg.size(); loop++)
        {
            v2.push_back(i_msg.at(loop));
        }

        zb_msg.Set( v1, v2 );
    }
    break;
  }
}

