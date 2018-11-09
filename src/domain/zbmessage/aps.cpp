/**
 * 
 */

#include "aps.h"

CAPSFrame::CAPSFrame()
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

std::vector<uint8_t> CAPSFrame::GetEmberAPS(void)
{
  std::vector<uint8_t> lo_aps;
  uint16_t l_option;

  lo_aps.push_back( profile_id & 0xFF );
  lo_aps.push_back( profile_id >> 8 );

  lo_aps.push_back( cluster_id & 0xFF );
  lo_aps.push_back( cluster_id >> 8 );

  lo_aps.push_back( src_ep );

  lo_aps.push_back( dest_ep );

  l_option = option.GetEmberApsOption();
  lo_aps.push_back( l_option & 0xFF );
  lo_aps.push_back( l_option >> 8 );

  lo_aps.push_back( group_id & 0xFF );
  lo_aps.push_back( group_id >> 8 );

  lo_aps.push_back( sequence );

  return lo_aps;
}

void CAPSFrame::SetEmberAPS(std::vector<uint8_t> i_data )
{
  uint8_t l_idx = 0;

  profile_id = (i_data.at(l_idx)&0xFF)|((i_data.at(l_idx+1)&0xFF)<<8);
  l_idx += 2;

  cluster_id = (i_data.at(l_idx)&0xFF)|((i_data.at(l_idx+1)&0xFF)<<8);
  l_idx += 2;

  src_ep = (i_data.at(l_idx)&0xFF);
  l_idx += 1;

  dest_ep = (i_data.at(l_idx)&0xFF);
  l_idx += 1;

  option.SetEmberApsOption( ((i_data.at(l_idx)&0xFF)|((i_data.at(l_idx+1)&0xFF)<<8)) );
  l_idx += 2;

  group_id = (i_data.at(l_idx)&0xFF)|((i_data.at(l_idx+1)&0xFF)<<8);
  l_idx += 2;

  sequence = i_data.at(l_idx)&0xFF;
  l_idx += 1;
}

