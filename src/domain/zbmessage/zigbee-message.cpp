/**
 * @file zigbee-message.cpp
 * 
 * @brief Handles encoding/decoding of a zigbee message
 */

#include "zigbee-message.h"

CZigBeeMsg::CZigBeeMsg() :
	aps(),
	zcl_header(),
	use_zcl_header(false),
	payload()
{
}

CZigBeeMsg::CZigBeeMsg(const CZigBeeMsg& i_msg) :
	aps(i_msg.aps),
	zcl_header(i_msg.zcl_header),
	use_zcl_header(i_msg.use_zcl_header),
	payload(i_msg.payload)
{
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
void CZigBeeMsg::SetSpecific( const uint16_t i_profile_id, const uint16_t i_manufacturer_code, const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint8_t i_cmd_id,
                              const EZCLFrameCtrlDirection i_direction, const std::vector<uint8_t>& i_payload,
                              const uint64_t i_src_ieee, const uint8_t i_transaction_number, const uint16_t i_grp_id )
{
  aps.SetDefaultAPS( i_profile_id, i_cluster_id, i_endpoint, i_grp_id );

  zcl_header = CZCLHeader();
  use_zcl_header = true;
  zcl_header.SetPublicSpecific( i_manufacturer_code, i_cmd_id, i_direction, i_transaction_number );

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
void CZigBeeMsg::SetGeneral(const uint16_t i_profile_id, const uint16_t i_manufacturer_code, const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint8_t i_cmd_id,
                 const EZCLFrameCtrlDirection i_direction, const std::vector<uint8_t>& i_payload , const uint64_t i_src_ieee,
                 const uint8_t i_transaction_number, const uint16_t i_grp_id)
{
  aps.SetDefaultAPS( i_profile_id, i_cluster_id, i_endpoint, i_grp_id );

  zcl_header = CZCLHeader();
  use_zcl_header = true;
  zcl_header.SetPublicGeneral( i_manufacturer_code, i_cmd_id, i_direction, i_transaction_number );

  payload = i_payload;
}

/**
 * @brief CZigBeeMsg::SetZdo : fill a ZDO message
 * @param i_cmd_id : ZDO command
 * @param i_payload : payload for command
 * @param i_transaction_number : transaction sequence number
 */
void CZigBeeMsg::SetZdo(const uint16_t i_cmd_id, const std::vector<uint8_t>& i_payload, const uint8_t i_transaction_number)
{
  aps.SetDefaultAPS( 0x0000, i_cmd_id, 0x00 );

  zcl_header = CZCLHeader();
  use_zcl_header = false;

  payload = i_payload;
  payload.insert( payload.begin(), i_transaction_number );
}


/**
 * @brief Set : parse an incomming raw EZSP message
 * @param i_aps : aps data
 * @param i_msg : message data included header (ZCL and/or MSP)
 */
void CZigBeeMsg::Set(const std::vector<uint8_t>& i_aps, const std::vector<uint8_t>& i_msg )
{
  uint8_t l_idx = 0;

  aps.SetEmberAPS( i_aps );

  // ZCL header
  // no ZCL Header for ZDO message
  if( 0U != aps.src_ep )
  {
    zcl_header = CZCLHeader(i_msg, l_idx);
    use_zcl_header = true;
  }
  else
  {
    use_zcl_header = false;
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
std::vector<uint8_t> CZigBeeMsg::Get( void ) const
{
  std::vector<uint8_t> lo_msg;

  if( use_zcl_header )
  {
    std::vector<uint8_t> v_tmp = zcl_header.GetZCLHeader();
    lo_msg.insert(lo_msg.end(), v_tmp.begin(), v_tmp.end());
  }

  lo_msg.insert(lo_msg.end(), payload.begin(), payload.end());

  return lo_msg;
}

/**
 * This method is a friend of CZigBeeMsg class
 * swap() is needed within operator=() to implement to copy and swap paradigm
**/
void swap(CZigBeeMsg& first, CZigBeeMsg& second) /* nothrow */
{
  using std::swap;	// Enable ADL

  swap(first.aps, second.aps);
  swap(first.zcl_header, second.zcl_header);
  swap(first.use_zcl_header, second.use_zcl_header);
  swap(first.payload, second.payload);
  /* Once we have swapped the members of the two instances... the two instances have actually been swapped */
}

CZigBeeMsg& CZigBeeMsg::operator=(CZigBeeMsg other)
{
  swap(*this, other);
  return *this;
}


