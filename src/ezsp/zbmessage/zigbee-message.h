/**
 * @file zigbee-message.h
 *
 * @brief Handles encoding/decoding of a zigbee message
 */

#pragma once

#include <cstdint>

#include <ezsp/zbmessage/zcl.h>

#include "ezsp/zbmessage/aps.h"
#include "ezsp/zbmessage/zclheader.h"
#include "spi/ByteBuffer.h"

namespace NSEZSP {

class CZigBeeMsg; /* Forward declaration */
void swap(CZigBeeMsg& first, CZigBeeMsg& second) noexcept; /* Declaration before qualifying ::swap() as friend for class CZigBeeMsg */

class CZigBeeMsg {
public:
	CZigBeeMsg();
	// high level

	/**
	 * @brief Build a basic cluster specific message
	 *
	 * @param i_profile_id
	 * @param i_manufacturer_code
	 * @param i_endpoint Destination endpoint
	 * @param i_cluster_id Concerned cluster
	 * @param i_cmd_id Command ID
	 * @param i_direction Model side
	 * @param i_payload Payload of the command
	 * @param i_transaction_number
	 * @param i_grp_id Multicast group address to use (0 is assume as unicast/broadcast)
	 */
	void SetSpecific(const uint16_t i_profile_id, const uint16_t i_manufacturer_code, const uint8_t i_endpoint,
					 const uint16_t i_cluster_id, const uint8_t i_cmd_id, const EZCLFrameCtrlDirection i_direction,
					 const NSSPI::ByteBuffer& i_payload, const uint8_t i_transaction_number, const uint16_t i_grp_id );

	/**
	 * @brief Build a basic cluster general message
	 *
	 * @param i_profile_id
	 * @param i_manufacturer_code
	 * @param i_endpoint Destination endpoint
	 * @param i_cluster_id Concerned cluster
	 * @param i_cmd_id Command ID
	 * @param i_direction Model side
	 * @param i_payload Payload of the command
	 * @param i_transaction_number
	 * @param i_grp_id Multicast group address to use (0 is assume as unicast/broadcast)
	 */
	void SetGeneral( const uint16_t i_profile_id, const uint16_t i_manufacturer_code, const uint8_t i_endpoint,
					 const uint16_t i_cluster_id, const uint8_t i_cmd_id, const EZCLFrameCtrlDirection i_direction,
					 const NSSPI::ByteBuffer& i_payload, const uint8_t i_transaction_number, const uint16_t i_grp_id );

	/**
	 * @brief Fill a ZDO message
	 *
	 * @param i_cmd_id : ZDO command
	 * @param i_payload : payload for command
	 * @param i_transaction_number : transaction sequence number
	 */
	void SetZdo(const uint16_t i_cmd_id, const NSSPI::ByteBuffer& i_payload, const uint8_t i_transaction_number = 0);

	/**
	 * @brief Getter for the enclosed APS frame
	 *
	 * @return The enclosed APS frame
	 */
	CAPSFrame GetAps() const {
		return aps;
	}

	/**
	 * @brief Getter for the enclosed ZCL header
	 *
	 * @return The enclosed ZCL header
	 */
	CZCLHeader GetZCLHeader() const {
		return zcl_header;
	}

	/**
	 * @brief Getter for the enclosed payload
	 *
	 * @return The enclosed payload
	 */
	NSSPI::ByteBuffer GetPayload() const {
		return payload;
	}

	/**
	 * @brief Parse an incomming raw EZSP message
	 *
	 * @param i_aps : aps data
	 * @param i_msg : message data included header (ZCL and/or MSP)
	 */
	void Set( const NSSPI::ByteBuffer& i_aps, const NSSPI::ByteBuffer& i_msg );

	/**
	 * @brief Get : format zigbee message frame with header
	 *
	 * @return Zigbee message with header
	 */
	NSSPI::ByteBuffer Get() const;

	/* FIXME: make the atribute below private as create getter/setter methods */
	CAPSFrame aps;        /*!< Enclosed APS frame */

private:
	CZCLHeader zcl_header;        /*!< Enclosed ZCL header */
	bool use_zcl_header;  /*!< Do we have a valid content in attribute zcl_header? */
	NSSPI::ByteBuffer payload; /*!< Enclosed payload */
};

} // namespace NSEZSP
