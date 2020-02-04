/**
 * @file aps.h
 *
 * @brief Handles encoding/decoding of the APS header
 */

#pragma once

#include <cstdint>

#include "ezsp/zbmessage/apsoption.h"
#include "spi/ByteBuffer.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

namespace NSEZSP {

class CAPSFrame
{
public:
  CAPSFrame();

  /**
   * @brief Default destructor
   */
  virtual ~CAPSFrame() = default;

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
  NSSPI::ByteBuffer GetEmberAPS(void);
  void SetEmberAPS( NSSPI::ByteBuffer i_data );

  // usefull
  /**
   * retrieve size in byte of APS structure
   */
  static uint8_t getSize(){ return 11; };

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

} // namespace NSEZSP
#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
