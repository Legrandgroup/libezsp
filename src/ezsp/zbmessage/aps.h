/**
 * @file aps.h
 *
 * @brief Handles encoding/decoding of the APS header
 */

#pragma once


#include <cstdint>
#include <vector>

#include "ezsp/zbmessage/apsoption.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

namespace NSEZSP {

class CAPSFrame; /* Forward declaration */
void swap(CAPSFrame& first, CAPSFrame& second) noexcept; /* Declaration before qualifying ::swap() as friend for class CAPSFrame */

class CAPSFrame
{
public:
  CAPSFrame();

  /**
   * @brief Copy constructor
   *
   * @param other The object to copy from
   */
  CAPSFrame(const CAPSFrame& other);

  /**
   * @brief Default destructor
   */
  virtual ~CAPSFrame(){}

  /**
   * @brief Assignment operator
   * @param other The object to assign to the lhs
   *
   * @return The object that has been assigned the value of \p other
   */
  CAPSFrame& operator=(CAPSFrame other);

  /**
   * @brief swap function to allow implementing of copy-and-swap idiom on members of type CAPSFrame
   *
   * This function will swap all attributes of \p first and \p second
   * See http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
   *
   * @param first The first object
   * @param second The second object
   */
  friend void swap(CAPSFrame& first, CAPSFrame& second) noexcept;

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
