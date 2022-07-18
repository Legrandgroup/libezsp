/**
 * @file zigbee-message.cpp
 *
 * @brief Handles encoding/decoding of a zigbee message
 */

#include "ezsp/zbmessage/zigbee-message.h"

using NSEZSP::CZigBeeMsg;

CZigBeeMsg::CZigBeeMsg() :
	aps(),
	zcl_header(),
	use_zcl_header(false),
	payload() {
}

void CZigBeeMsg::SetSpecific( const uint16_t i_profile_id, const uint16_t i_manufacturer_code, const uint8_t i_endpoint,
							  const uint16_t i_cluster_id, const uint8_t i_cmd_id, const EZCLFrameCtrlDirection i_direction,
							  const NSSPI::ByteBuffer& i_payload, const uint8_t i_transaction_number, const uint16_t i_grp_id ) {
	aps.SetDefaultAPS( i_profile_id, i_cluster_id, i_endpoint, i_grp_id );
	zcl_header = CZCLHeader();
	use_zcl_header = true;
	zcl_header.SetPublicSpecific( i_manufacturer_code, i_cmd_id, i_direction, i_transaction_number );
	payload = i_payload;
}

void CZigBeeMsg::SetGeneral( const uint16_t i_profile_id, const uint16_t i_manufacturer_code, const uint8_t i_endpoint,
							 const uint16_t i_cluster_id, const uint8_t i_cmd_id, const EZCLFrameCtrlDirection i_direction,
							 const NSSPI::ByteBuffer& i_payload, const uint8_t i_transaction_number, const uint16_t i_grp_id ) {
	aps.SetDefaultAPS( i_profile_id, i_cluster_id, i_endpoint, i_grp_id );
	zcl_header = CZCLHeader();
	use_zcl_header = true;
	zcl_header.SetPublicGeneral( i_manufacturer_code, i_cmd_id, i_direction, i_transaction_number );
	payload = i_payload;
}

void CZigBeeMsg::SetZdo(const uint16_t i_cmd_id, const NSSPI::ByteBuffer& i_payload, const uint8_t i_transaction_number) {
	aps.SetDefaultAPS( 0x0000, i_cmd_id, 0x00 );
	zcl_header = CZCLHeader();
	use_zcl_header = false;
	payload = i_payload;
	payload.insert( payload.begin(), i_transaction_number );
}

void CZigBeeMsg::Set(const NSSPI::ByteBuffer& i_aps, const NSSPI::ByteBuffer& i_msg ) {
	uint8_t l_idx = 0;
	aps.SetEmberAPS( i_aps );

	// ZCL header
	// no ZCL Header for ZDO message
	if( 0U != aps.src_ep ) {
		zcl_header = CZCLHeader(i_msg, l_idx);
		use_zcl_header = true;
	}
	else {
		use_zcl_header = false;
	}

	// payload
	for(uint16_t loop=l_idx; loop<i_msg.size(); loop++) {
		payload.push_back(i_msg.at(loop));
	}
}

NSSPI::ByteBuffer CZigBeeMsg::Get( void ) const {
	NSSPI::ByteBuffer lo_msg;
	if( use_zcl_header ) {
		NSSPI::ByteBuffer v_tmp = zcl_header.GetZCLHeader();
		lo_msg.insert(lo_msg.end(), v_tmp.begin(), v_tmp.end());
	}
	lo_msg.insert(lo_msg.end(), payload.begin(), payload.end());
	return lo_msg;
}
