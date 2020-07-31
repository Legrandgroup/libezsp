/**
 * @file zclheader.cpp
 *
 * @brief Handles encoding/decoding of ZCL headers
 */

#include "ezsp/zbmessage/zclheader.h"

#include "ezsp/byte-manip.h"

using NSEZSP::CZCLHeader;

CZCLHeader::CZCLHeader() :
	frm_ctrl(),
	manufacturer_code(LG_MAN_CODE),
	transaction_number(0),
	cmd_id(0) {
}

/**
 * The i_data buffer contains the input bytes to decode.
 * First byte is the frm_ctrl
 * If manufacturer code is present, is is located in a 2-bytes value at offsets 1 and 2
 * followed by a 1-byte transaction number
 * followed by a 1-byte command id
 *
 * Total of bytes expected: 5 (if manufacturer code is present) or 3 otherwise
 */
CZCLHeader::CZCLHeader(const NSSPI::ByteBuffer& i_data, uint8_t& o_idx) :
	frm_ctrl(i_data.at(0)),
	manufacturer_code(frm_ctrl.IsManufacturerCodePresent()?dble_u8_to_u16(i_data.at(2), i_data.at(1)):0),
	transaction_number(frm_ctrl.IsManufacturerCodePresent()?i_data.at(3):i_data.at(1)),
	cmd_id(frm_ctrl.IsManufacturerCodePresent()?i_data.at(4):i_data.at(2)) {
	if( frm_ctrl.IsManufacturerCodePresent() ) {
		o_idx=5;	/* When manufacturer code is present, the header is 5-bytes long */
	}
	else {
		o_idx=3;	/* otherwise, it is 3-bytes long */
	}
}

/**
 * @brief SetMSPSpecific : build default msp cluster specific ZCL header
 * @param i_cmd_id       : command identifier
 * @param i_direction    : model direction
 */
void CZCLHeader::SetMSPSpecific( const uint16_t i_profile_id, const uint8_t i_cmd_id, const EZCLFrameCtrlDirection i_direction, const uint8_t i_transaction_number ) {
	cmd_id = i_cmd_id;
	frm_ctrl.SetDirection( i_direction );
	transaction_number = i_transaction_number;
}

/**
 * @brief SetMSPGeneral  : build default msp cluster general ZCL header
 * @param i_profile_id
 * @param i_cmd_id       : command identifier
 * @param i_direction    : model direction
 * @param i_transaction_number
 */
void CZCLHeader::SetMSPGeneral( const uint16_t i_profile_id, const uint8_t i_cmd_id,
                                const EZCLFrameCtrlDirection i_direction, const uint8_t i_transaction_number ) {
	cmd_id = i_cmd_id;
	frm_ctrl.SetDirection( i_direction );
	frm_ctrl.SetFrameType( E_FRM_TYPE_GENERAL );
	transaction_number = i_transaction_number;
}

/**
 * @brief SetPublicSpecific : build default public cluster specific ZCL header
 * @param i_cmd_id       : command identifier
 * @param i_direction    : model direction
 */
void CZCLHeader::SetPublicSpecific( const uint16_t i_manufacturer_id, const uint8_t i_cmd_id, const EZCLFrameCtrlDirection i_direction, const uint8_t i_transaction_number ) {
	cmd_id = i_cmd_id;
	frm_ctrl.SetDirection( i_direction );
	transaction_number = i_transaction_number;
	if( 0xFFFFU == i_manufacturer_id ) {
		frm_ctrl.SetManufacturerCodePresent( false );
	}
	manufacturer_code = i_manufacturer_id;
}

/**
 * @brief SetPublicGeneral  : build default public cluster general ZCL header
 * @param i_manufacturer_id
 * @param i_cmd_id       : command identifier
 * @param i_direction    : model direction
 * @param i_transaction_number
 */
void CZCLHeader::SetPublicGeneral( const uint16_t i_manufacturer_id, const uint8_t i_cmd_id,
                                   const EZCLFrameCtrlDirection i_direction, const uint8_t i_transaction_number ) {
	cmd_id = i_cmd_id;
	frm_ctrl.SetDirection( i_direction );
	frm_ctrl.SetFrameType( E_FRM_TYPE_GENERAL );
	transaction_number = i_transaction_number;
	if( 0xFFFFU == i_manufacturer_id ) {
		frm_ctrl.SetManufacturerCodePresent( false );
	}
	manufacturer_code = i_manufacturer_id;
}

NSSPI::ByteBuffer CZCLHeader::GetZCLHeader(void) const {
	NSSPI::ByteBuffer lo_data;

	lo_data.push_back(frm_ctrl.GetFrmCtrlByte());
	if( frm_ctrl.IsManufacturerCodePresent() ) {
		lo_data.push_back( u16_get_lo_u8(manufacturer_code) );
		lo_data.push_back( u16_get_hi_u8(manufacturer_code) );
	}
	lo_data.push_back(transaction_number);
	lo_data.push_back(cmd_id);

	return lo_data;
}
