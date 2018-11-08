/**
 * 
 */

#pragma once


#include <cstdint>
#include <vector>

#include "apsoption.h"

class CAPSFrame
{
public:
  CAPSFrame();

  // hight level
  /**
   * @brief SetDefaultAPS : configure a default aps
   * @param i_profile_id  : profile to use
   * @param i_cluster_id  : cluster concerned
   * @param i_dest_ep     : destination endpoin
   * @param i_grp_id      : group id if necessary
   */
  void SetDefaultAPS( uint16_t i_profile_id, uint16_t i_cluster_id, uint8_t i_dest_ep, uint16_t i_grp_id = 0 );

  // concatenate
  std::vector<uint8_t> GetEmberAPS(void);
  void SetEmberAPS( std::vector<uint8_t> i_data );

  // variables
  /** */
  uint16_t cluster_id;
  /** */
  uint8_t dest_ep;
  /** */
  uint16_t group_id;
  /** */
  CAPSOption option;
  /** */
  uint16_t profile_id;
  /** */
  uint8_t sequence;
  /** */
  uint8_t src_ep;
};

