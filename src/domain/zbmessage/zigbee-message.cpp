/**
 * 
 */

#include "zigbee-message.h"

CZigBeeMsg::CZigBeeMsg()
{
  p_zcl_header = nullptr;
}

CZigBeeMsg::CZigBeeMsg(const CZigBeeMsg& i_msg)
{
  // copy all elements of class
  /** APS */
  aps = i_msg.aps;

  /** ZCL Header */
  p_zcl_header = nullptr;
  if( nullptr != i_msg.p_zcl_header )
  {
    p_zcl_header = new CZCLHeader();
    *p_zcl_header = *i_msg.p_zcl_header;
  }

  /** Payload */
  payload.clear();
  payload = i_msg.payload;
}

CZigBeeMsg::~CZigBeeMsg()
{
  if( nullptr != p_zcl_header )
  {
    delete p_zcl_header;
    p_zcl_header = nullptr;
  }
}

/**
 * @brief SetSpecific   : build a basic cluster specific message
 * @param i_profile_id  : identifier of ZigBee profile to use
 * @param i_manufacturer_code  : set to 0xFFFF if public cluster
 * @param i_endpoint    : destination endpoint
 * @param i_cluster_id  : concerned cluster
 * @param i_cmd_id      : command
 * @param i_direction   : model side
 * @param i_payload     : payload of command
 * @param i_src_ieee    : address ieee to use as source of message
 * @param i_grp_id      : multicast group address to use (0 is assume as unicast/broadcast)
 */
void CZigBeeMsg::SetSpecific( uint16_t i_profile_id, uint16_t i_manufacturer_code, uint8_t i_endpoint, uint16_t i_cluster_id, uint8_t i_cmd_id,
                              EZCLFrameCtrlDirection i_direction, std::vector<uint8_t> i_payload,
                              uint64_t i_src_ieee, uint8_t i_transaction_number, uint16_t i_grp_id )
{
  aps.SetDefaultAPS( i_profile_id, i_cluster_id, i_endpoint, i_grp_id );

  if( nullptr != p_zcl_header )
  {
    delete p_zcl_header;
  }
  p_zcl_header = new CZCLHeader();

  p_zcl_header->SetPublicSpecific( i_manufacturer_code, i_cmd_id, i_direction, i_transaction_number );

  payload = i_payload;
}

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
void CZigBeeMsg::SetGeneral(uint16_t i_profile_id, uint16_t i_manufacturer_code, uint8_t i_endpoint, uint16_t i_cluster_id, uint8_t i_cmd_id,
                 EZCLFrameCtrlDirection i_direction, std::vector<uint8_t> i_payload , uint64_t i_src_ieee,
                 uint8_t i_transaction_number, uint16_t i_grp_id)
{
  aps.SetDefaultAPS( i_profile_id, i_cluster_id, i_endpoint, i_grp_id );

  if( nullptr != p_zcl_header )
  {
    delete p_zcl_header;
  }
  p_zcl_header = new CZCLHeader();

  p_zcl_header->SetPublicGeneral( i_manufacturer_code, i_cmd_id, i_direction, i_transaction_number );

  payload = i_payload;
}

/**
 * @brief CZigBeeMsg::SetZdo : fill a ZDO message
 * @param i_cmd_id : ZDO command
 * @param i_payload : payload for command
 * @param i_transaction_number : transaction sequence number
 */
void CZigBeeMsg::SetZdo(uint16_t i_cmd_id, std::vector<uint8_t> i_payload, uint8_t i_transaction_number)
{
  aps.SetDefaultAPS( 0x0000, i_cmd_id, 0x00 );

  if( nullptr != p_zcl_header )
  {
    delete p_zcl_header;
  }

  payload = i_payload;
  payload.insert( payload.begin(), i_transaction_number );
}


/**
 * @brief Set : parse an incomming raw EZSP message
 * @param i_aps : aps data
 * @param i_msg : message data included header (ZCL and/or MSP)
 */
void CZigBeeMsg::Set(std::vector<uint8_t> i_aps, std::vector<uint8_t> i_msg )
{
  uint8_t l_idx = 0;

  aps.SetEmberAPS( i_aps );

  // ZCL header
  if( nullptr != p_zcl_header )
  {
    delete p_zcl_header;
  }
  // no ZCL Header for ZDO message
  if( 0U != aps.src_ep )
  {
    p_zcl_header = new CZCLHeader();
    l_idx = p_zcl_header->SetZCLHeader( i_msg );
  }

  // payload
  for(uint16_t loop=l_idx; loop<i_msg.size(); loop++)
  {
      payload.push_back(i_msg.at(loop));
  }  
}

/**
 * @brief Get : format zigbee message frame with header
 * @return return zigbee message with header
 */
std::vector<uint8_t> CZigBeeMsg::Get( void )
{
  std::vector<uint8_t> lo_msg;

  if( nullptr != p_zcl_header )
  {
    std::vector<uint8_t> v_tmp = p_zcl_header->GetZCLHeader();
    lo_msg.insert(lo_msg.end(), v_tmp.begin(), v_tmp.end());
  }

  lo_msg.insert(lo_msg.end(), payload.begin(), payload.end());

  return lo_msg;
}

// operator
CZigBeeMsg& CZigBeeMsg::operator= (const CZigBeeMsg& i_msg)
{
  // copy all elements of class
  /** APS */
  aps = i_msg.aps;

  /** ZCL Header */
  if( nullptr != p_zcl_header )
  {
    delete p_zcl_header;
    p_zcl_header = nullptr;
  }
  if( nullptr != i_msg.p_zcl_header )
  {
    p_zcl_header = new CZCLHeader();
    *p_zcl_header = *i_msg.p_zcl_header;
  }

  /** Payload */
  payload.clear();
  payload = i_msg.payload;

  return *this;
}

