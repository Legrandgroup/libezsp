/**
 * @file apsoption.cpp
 * 
 * @brief Handles encoding/decoding of the 16-bit APS option word
 */

#include "apsoption.h"


using namespace NSEZSP;

CAPSOption::CAPSOption() :
	dest_ieee(false),
	enable_addr_discovery(true),
	enable_route_discovery(true),
	encryption(false),
	force_route_discovery(false),
	fragment(false),
	retry(true),
	src_ieee(true),
	zdo_rsp_requiered(false)
{
}

CAPSOption::CAPSOption(const CAPSOption& other) :
	dest_ieee(other.dest_ieee),
	enable_addr_discovery(other.enable_addr_discovery),
	enable_route_discovery(other.enable_route_discovery),
	encryption(other.encryption),
	force_route_discovery(other.force_route_discovery),
	fragment(other.fragment),
	retry(other.retry),
	src_ieee(other.src_ieee),
	zdo_rsp_requiered(other.zdo_rsp_requiered)
{
}

uint16_t CAPSOption::GetEmberApsOption(void) const
{
  uint16_t lo_option = 0;

  if (encryption) {
      lo_option |= 0x0020;
  }
  if (retry) {
      lo_option |= 0x0040;
  }
  if (enable_route_discovery) {
      lo_option |= 0x0100;
  }
  if (force_route_discovery) {
      lo_option |= 0x0200;
  }
  if (src_ieee) {
      lo_option |= 0x0400;
  }
  if (dest_ieee) {
      lo_option |= 0x0800;
  }
  if (enable_addr_discovery) {
      lo_option |= 0x1000;
  }
  if (zdo_rsp_requiered) {
      lo_option |= 0x4000;
  }
  if (fragment) {
      lo_option |= 0x8000;
  }

  return lo_option;
}

void CAPSOption::SetEmberApsOption( const uint16_t i_option )
{
  encryption = (0!=(i_option&0x0020));
  retry = (0!=(i_option&0x0040));
  enable_route_discovery = (0!=(i_option&0x0100));
  force_route_discovery = (0!=(i_option&0x0200));
  src_ieee = (0!=(i_option&0x0400));
  dest_ieee = (0!=(i_option&0x0800));
  enable_addr_discovery = (0!=(i_option&0x1000));
  zdo_rsp_requiered = (0!=(i_option&0x4000));
  fragment = (0!=(i_option&0x8000));
}

/**
 * This method is a friend of CAPSOption class
 * swap() is needed within operator=() to implement to copy and swap paradigm
**/
void NSEZSP::swap(CAPSOption& first, CAPSOption& second) noexcept /* nothrow */
{
  using std::swap;	// Enable ADL

  swap(first.dest_ieee, second.dest_ieee);
  swap(first.enable_addr_discovery, second.enable_addr_discovery);
  swap(first.enable_route_discovery, second.enable_route_discovery);
  swap(first.encryption, second.encryption);
  swap(first.force_route_discovery, second.force_route_discovery);
  swap(first.fragment, second.fragment);
  swap(first.retry, second.retry);
  swap(first.src_ieee, second.src_ieee);
  swap(first.zdo_rsp_requiered, second.zdo_rsp_requiered);
  /* Once we have swapped the members of the two instances... the two instances have actually been swapped */
}

CAPSOption& CAPSOption::operator=(CAPSOption other)
{
  swap(*this, other);
  return *this;
}

