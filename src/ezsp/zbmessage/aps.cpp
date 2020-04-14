/**
 * @file aps.cpp
 *
 * @brief Handles encoding/decoding of the APS header
 */

#include "ezsp/zbmessage/aps.h"
#include "ezsp/byte-manip.h"

using NSEZSP::CAPSFrame;

CAPSFrame::CAPSFrame() : cluster_id(0), dest_ep(0), group_id(0), option(), profile_id(0), sequence(0), src_ep(0)
{
}

/**
 * @brief SetDefaultAPS : configure a default aps
 * @param i_profile_id  : profile to use
 * @param i_cluster_id  : cluster concerned
 * @param i_dest_ep     : destination endpoin
 * @param i_grp_id      : group id if necessary
 */
void CAPSFrame::SetDefaultAPS( uint16_t i_profile_id, uint16_t i_cluster_id, uint8_t i_dest_ep, uint16_t i_grp_id )
{
  profile_id = i_profile_id;
  cluster_id = i_cluster_id;
  dest_ep = i_dest_ep;
  group_id = i_grp_id;
  sequence = 0;
  src_ep = 1;

}

NSSPI::ByteBuffer CAPSFrame::GetEmberAPS(void)
{
  NSSPI::ByteBuffer lo_aps;
  uint16_t l_option;

  lo_aps.push_back( u16_get_lo_u8(profile_id) );
  lo_aps.push_back( u16_get_hi_u8(profile_id) );

  lo_aps.push_back( u16_get_lo_u8(cluster_id) );
  lo_aps.push_back( u16_get_hi_u8(cluster_id) );

  lo_aps.push_back( src_ep );

  lo_aps.push_back( dest_ep );

  l_option = option.GetEmberApsOption();
  lo_aps.push_back( u16_get_lo_u8(l_option) );
  lo_aps.push_back( u16_get_hi_u8(l_option) );

  lo_aps.push_back( u16_get_lo_u8(group_id) );
  lo_aps.push_back( u16_get_hi_u8(group_id) );

  lo_aps.push_back( sequence );

  return lo_aps;
}

void CAPSFrame::SetEmberAPS(NSSPI::ByteBuffer i_data)
{
  uint8_t l_idx = 0;

  profile_id = dble_u8_to_u16(i_data.at(l_idx+1U), i_data.at(l_idx));
  l_idx++;
  l_idx++;

  cluster_id = dble_u8_to_u16(i_data.at(l_idx+1U), i_data.at(l_idx));
  l_idx++;
  l_idx++;

  src_ep = i_data.at(l_idx++);

  dest_ep = i_data.at(l_idx++);

  option.SetEmberApsOption( dble_u8_to_u16(i_data.at(l_idx+1U), i_data.at(l_idx)) );
  l_idx++;
  l_idx++;

  group_id = dble_u8_to_u16(i_data.at(l_idx+1U), i_data.at(l_idx));
  l_idx++;
  l_idx++;

  sequence = i_data.at(l_idx++);
}
