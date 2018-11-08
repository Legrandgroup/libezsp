#ifndef OUT_ZIGBEE_MESSAGE_H
#define OUT_ZIGBEE_MESSAGE_H

#include <cstdint>
#include <vector>

#include "zigbee-message.h"

typedef enum
{
  E_ZB_MSG_READY_TO_SEND,
  E_ZB_MSG_WAITING_EZSP_ACK,
  E_ZB_MSG_WAITING_ZB_ACK,
}EOutMsgState;

typedef enum
{
  E_OUT_MSG_TYPE_DIRECT_UNICAST,
  E_OUT_MSG_TYPE_INDIRECT_UNICAT,
  E_OUT_MSG_TYPE_BROADCAST,
  E_OUT_MSG_TYPE_MULTICAST
}EOutMsgType;

typedef enum
{
  E_OUT_MSG_BR_DEST_ALL_DEVICES = 0xFFFF,
  E_OUT_MSG_BR_DEST_NON_SLEEPY = 0xFFFD,
  E_OUT_MSG_BR_DEST_ROUTER_ONLY = 0xFFFC
}EOutBroadcastDestination;


class COutZbMessage
{
public:
  COutZbMessage(){}

  /**
   * @brief COutZbMessage : constructeur with full EZSP dongle message
   * @param i_msg : message to parse
   */
  COutZbMessage( std::vector<uint8_t> i_msg ) { SetEZSPMessage(i_msg); }

  // state
  EOutMsgState GetState( void ){ return state; }
  void SetState( EOutMsgState i_state );

  // timeout
  uint16_t GetTimeout( void ){ return timeout_timer; }
  /**
   * @brief DecTimeout : decrement timeout value
   * @param i_timeout : value of decrement in ms
   */
  void DecTimeout( uint16_t i_dec_value );

  // sequence number
  void SetSeqNumber( uint8_t i_seq_nb ) { zb_msg.GetAps()->sequence = i_seq_nb; }

  // tag
  void SetTag( uint8_t i_tag ){ msg_tag = i_tag; }
  uint8_t GetTag( void ){ return msg_tag; }

  // message
  /**
   * @brief SetDirectUnicastMessage : prepare to send a direct unicast message
   * @param i_dest_node_id : destination
   * @param i_msg_tag : user tag
   * @param i_msg : zigbee message
   */
  void SetDirectUnicastMessage(uint16_t i_dest_node_id, CZigBeeMsg i_msg );

  /**
   * @brief SetBroadcastMessage : prepare to sending a broadcast message
   * @param i_destination : type of node concern by broadcast
   * @param radius : The message will be delivered to all nodes within radius hops of the sender.
   *                  A radius of zero is converted to EMBER_MAX_HOPS.
   * @param i_msg_tag : user tag
   * @param i_msg : zigbee message
   */
  void SetBroadcastMessage(EOutBroadcastDestination i_destination, uint8_t i_radius, CZigBeeMsg i_msg );

  /**
   * @brief GetZbMsg : access to private variable
   * @return return pointer to ZigBee Message
   */
  CZigBeeMsg *GetZbMsg( void ){ return &zb_msg; }

  /**
   * @brief GetMsgType : return type of message
   * @return
   */
  EOutMsgType GetMsgType(void){return msg_type;}

  // ezsp concatenante
  /**
   * @brief GetEZSPMessage : concatenate message for EZSP protocol
   * @return : return a well form buffer payload with EZSP command
   */
  std::vector<uint8_t> GetEZSPMessage(void);

  /**
   * @brief SetEZSPMessage
   * @param i_msg
   */
  void SetEZSPMessage( std::vector<uint8_t> i_msg );

  uint16_t GetUnicastDestination( void ){ return unicast_dest_node_id; }

private:
  /** zigbee message */
  CZigBeeMsg zb_msg;
  /** sending state */
  EOutMsgState state;
  /** current timeout timer in ms */
  uint16_t timeout_timer;
  /** message tag */
  uint8_t msg_tag;
  /** type of message */
  EOutMsgType msg_type;
  /** unicast dest node id */
  uint16_t unicast_dest_node_id;
  /** unicast dest ieee */
  uint64_t unicast_dest_ieee;
  /** broadcast destination */
  EOutBroadcastDestination broadcast_dest;
  /** broadcast radius : 0 for max hop*/
  uint8_t broadcast_radius;
  /** multicast hops : The message will be delivered to all nodes within this number of hops of the sender. A value of zero is converted to EMBER_MAX_HOPS. */
  uint8_t multicast_hops;
  /** multicast non member radius : The number of hops that the message will be forwarded by devices that are not members of the group. A value of 7 or greater is treated as infinite. */
  uint8_t multicast_non_member_radius;
};


#endif // OUT_ZIGBEE_MESSAGE_H
