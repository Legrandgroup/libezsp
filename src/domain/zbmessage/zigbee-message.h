#ifndef ZIGBEE_MESSAGE_H
#define ZIGBEE_MESSAGE_H

#include <cstdint>
#include <vector>

#include "aps.h"
#include "zclheader.h"

class CZigBeeMsg
{
public:
  CZigBeeMsg();
  CZigBeeMsg(const CZigBeeMsg& i_msg);
  ~CZigBeeMsg();

  // high level

  /**
   * @brief SetSpecific   : build a basic cluster specific message
   * @param i_msp_nwk     : indicate if it's an msp or public message
   * @param i_endpoint    : destination endpoint
   * @param i_cluster_id  : concerned cluster
   * @param i_cmd_id      : command
   * @param i_direction   : model side
   * @param i_payload     : payload of command
   * @param i_src_ieee    : address ieee to use as source of message
   * @param i_grp_id      : multicast group address to use (0 is assume as unicast/broadcast)
   */
  void SetSpecific(uint16_t i_profile_id, uint16_t i_manufacturer_code, uint8_t i_endpoint, uint16_t i_cluster_id, uint8_t i_cmd_id,
                    EZCLFrameCtrlDirection i_direction, std::vector<uint8_t> i_payload , uint64_t i_src_ieee,
                    uint8_t i_transaction_number = 0, uint16_t i_grp_id = 0);

  /**
   * @brief SetGeneral   : build a basic cluster general message
   * @param i_msp_nwk     : indicate if it's an msp or public message
   * @param i_endpoint    : destination endpoint
   * @param i_cluster_id  : concerned cluster
   * @param i_cmd_id      : command
   * @param i_direction   : model side
   * @param i_payload     : payload of command
   * @param i_src_ieee    : address ieee to use as source of message
   * @param i_grp_id      : multicast group address to use (0 is assume as unicast/broadcast)
   */
  void SetGeneral(uint16_t i_profile_id, uint16_t i_manufacturer_code, uint8_t i_endpoint, uint16_t i_cluster_id, uint8_t i_cmd_id,
                   EZCLFrameCtrlDirection i_direction, std::vector<uint8_t> i_payload , uint64_t i_src_ieee,
                   uint8_t i_transaction_number = 0, uint16_t i_grp_id = 0 );

  /**
   * @brief CZigBeeMsg::SetZdo : fill a ZDO message
   * @param i_cmd_id : ZDO command
   * @param i_payload : payload for command
   * @param i_transaction_number : transaction sequence number
   */
  void SetZdo(uint16_t i_cmd_id, std::vector<uint8_t> i_payload, uint8_t i_transaction_number = 0);

  // aps
  CAPSFrame *GetAps(void) { return &aps; }

  // ZCL Header
  CZCLHeader *GetZCLHeader(void){ return p_zcl_header; }

  // payload
  std::vector<uint8_t> *GetPaylaod(void){ return &payload; }

  // concatenate
  /**
   * @brief Set : parse an incomming raw EZSP message
   * @param i_aps : aps data
   * @param i_msg : message data included header (ZCL and/or MSP)
   */
  void Set( std::vector<uint8_t> i_aps, std::vector<uint8_t> i_msg );

  /**
   * @brief Get : format zigbee message frame with header
   * @return return zigbee message with header
   */
  std::vector<uint8_t> Get( void );

  // operator
  CZigBeeMsg& operator= (const CZigBeeMsg& i_msg);

private:
  /** APS */
  CAPSFrame aps;

  /** ZCL Header */
  CZCLHeader *p_zcl_header;

  /** Payload */
  std::vector<uint8_t> payload;
};

#endif // ZIGBEE_MESSAGE_H
