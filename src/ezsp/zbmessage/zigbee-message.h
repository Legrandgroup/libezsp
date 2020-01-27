/**
 * @file zigbee-message.h
 *
 * @brief Handles encoding/decoding of a zigbee message
 */

#pragma once

#include <cstdint>
#include <vector>

#include <ezsp/zbmessage/zcl.h>

#include "ezsp/zbmessage/aps.h"
#include "ezsp/zbmessage/zclheader.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

namespace NSEZSP {

class CZigBeeMsg; /* Forward declaration */
void swap(CZigBeeMsg& first, CZigBeeMsg& second) noexcept; /* Declaration before qualifying ::swap() as friend for class CZigBeeMsg */

class CZigBeeMsg
{
public:
  CZigBeeMsg();
  // high level

  /**
   * @brief Build a basic cluster specific message
   * @param i_msp_nwk     : indicate if it's an msp or public message
   * @param i_endpoint    : destination endpoint
   * @param i_cluster_id  : concerned cluster
   * @param i_cmd_id      : command
   * @param i_direction   : model side
   * @param i_payload     : payload of command
   * @param i_src_ieee    : address ieee to use as source of message
   * @param i_grp_id      : multicast group address to use (0 is assume as unicast/broadcast)
   */
  void SetSpecific(const uint16_t i_profile_id, const uint16_t i_manufacturer_code, const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint8_t i_cmd_id,
                    const EZCLFrameCtrlDirection i_direction, const std::vector<uint8_t>& i_payload , const uint64_t i_src_ieee,
                    const uint8_t i_transaction_number = 0, const uint16_t i_grp_id = 0);

  /**
   * @brief Build a basic cluster general message
   * @param i_msp_nwk     : indicate if it's an msp or public message
   * @param i_endpoint    : destination endpoint
   * @param i_cluster_id  : concerned cluster
   * @param i_cmd_id      : command
   * @param i_direction   : model side
   * @param i_payload     : payload of command
   * @param i_src_ieee    : address ieee to use as source of message
   * @param i_grp_id      : multicast group address to use (0 is assume as unicast/broadcast)
   */
  void SetGeneral(const uint16_t i_profile_id, const uint16_t i_manufacturer_code, const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint8_t i_cmd_id,
                   const EZCLFrameCtrlDirection i_direction, const std::vector<uint8_t>& i_payload , const uint64_t i_src_ieee,
                   const uint8_t i_transaction_number = 0, const uint16_t i_grp_id = 0 );

  /**
   * @brief Fill a ZDO message
   * @param i_cmd_id : ZDO command
   * @param i_payload : payload for command
   * @param i_transaction_number : transaction sequence number
   */
  void SetZdo(const uint16_t i_cmd_id, const std::vector<uint8_t>& i_payload, const uint8_t i_transaction_number = 0);

  /**
   * @brief Getter for the enclosed APS frame
   *
   * @return The enclosed APS frame
   */
  CAPSFrame GetAps() const { return aps; }

  /**
   * @brief Getter for the enclosed ZCL header
   *
   * @return The enclosed ZCL header
   */
  CZCLHeader GetZCLHeader() const { return zcl_header; }

  /**
   * @brief Getter for the enclosed payload
   *
   * @return The enclosed payload
   */
  std::vector<uint8_t> GetPayload() const { return payload; }

  /**
   * @brief Parse an incomming raw EZSP message
   * @param i_aps : aps data
   * @param i_msg : message data included header (ZCL and/or MSP)
   */
  void Set( const std::vector<uint8_t>& i_aps, const std::vector<uint8_t>& i_msg );

  /**
   * @brief Get : format zigbee message frame with header
   * @return Zigbee message with header
   */
  std::vector<uint8_t> Get() const;

  /* FIXME: make the atribute below private as create getter/setter methods */
  CAPSFrame aps;        /*!< Enclosed APS frame */
private:
  CZCLHeader zcl_header;        /*!< Enclosed ZCL header */
  bool use_zcl_header;  /*!< Do we have a valid content in attribute zcl_header? */
  std::vector<uint8_t> payload; /*!< Enclosed payload */
};

} // namespace NSEZSP

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
