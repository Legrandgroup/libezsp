/**
 * @file zclheader.h
 *
 * @brief Handles encoding/decoding of ZCL headers
 */

#pragma once

#include <cstdint>

#include <ezsp/zbmessage/zclframecontrol.h>
#include "spi/ByteBuffer.h"

// manufacturer code
constexpr uint16_t PUBLIC_CODE = 0xFFFF;
constexpr uint16_t LG_MAN_CODE = 0x1021;

// profile
constexpr uint16_t GP_PROFILE_ID = 0xA1E0;

namespace NSEZSP {

class CZCLHeader; /* Forward declaration */
void swap(CZCLHeader& first, CZCLHeader& second) noexcept; /* Declaration before qualifying ::swap() as friend for class CZCLHeader */

class CZCLHeader {
public:
	/**
	 * @brief Default constructor
	 */
	CZCLHeader();

	/**
	 * @brief Default destructor
	 */
	virtual ~CZCLHeader() = default;

	/**
	 * @brief Constructor from a buffer
	 *
	 * @param[in] i_data The buffer to parse in order to construct this instance
	 * @param[out] o_idx The number of bytes used (in buffer i_data) to construct the ZCL header
	 */
	CZCLHeader(const NSSPI::ByteBuffer& i_data, uint8_t& o_idx);

	// high level

	/**
	 * @brief SetMSPSpecific : build default msp cluster specific ZCL header
	 * @param i_cmd_id       : command identifier
	 * @param i_direction    : model direction
	 */
	void SetMSPSpecific( const uint16_t i_profile_id, const uint8_t i_cmd_id,
	                     const EZCLFrameCtrlDirection i_direction, const uint8_t i_transaction_number = 0 );

	/**
	 * @brief SetMSPGeneral  : build default msp cluster general ZCL header
	 * @param i_profile_id
	 * @param i_cmd_id       : command identifier
	 * @param i_direction    : model direction
	 * @param i_transaction_number
	 */
	void SetMSPGeneral( const uint16_t i_profile_id, const uint8_t i_cmd_id,
	                    const EZCLFrameCtrlDirection i_direction, const uint8_t i_transaction_number = 0 );

	/**
	 * @brief SetPublicSpecific : build default public cluster specific ZCL header
	 * @param i_cmd_id       : command identifier
	 * @param i_direction    : model direction
	 */
	void SetPublicSpecific( const uint16_t i_manufacturer_id, const uint8_t i_cmd_id,
	                        const EZCLFrameCtrlDirection i_direction, const uint8_t i_transaction_number );

	/**
	 * @brief SetPublicGeneral  : build default public cluster general ZCL header
	 * @param i_manufacturer_id
	 * @param i_cmd_id       : command identifier
	 * @param i_direction    : model direction
	 * @param i_transaction_number
	 */
	void SetPublicGeneral(const uint16_t i_manufacturer_id, const uint8_t i_cmd_id,
	                      const EZCLFrameCtrlDirection i_direction, const uint8_t i_transaction_number );

	// frame control
	CZCLFrameControl GetFrmCtrl(void) const {
		return frm_ctrl;
	}
	void SetFrameControl( const CZCLFrameControl& i_frm_ctrl ) {
		frm_ctrl=i_frm_ctrl ;
	}

	// manufacturer code
	uint16_t GetManCode(void) const {
		return manufacturer_code;
	}
	void SetManCode( const uint16_t i_code ) {
		manufacturer_code = i_code;
	}

	// transaction number
	uint8_t GetTransactionNb(void) const {
		return transaction_number;
	}
	void SetTransactionNb( const uint8_t i_tr_nb ) {
		transaction_number = i_tr_nb;
	}

	// command id
	uint8_t GetCmdId( void ) const {
		return cmd_id;
	}
	void SetCmdId( const uint8_t i_cmd_id ) {
		cmd_id = i_cmd_id;
	}

	// concatenate
	NSSPI::ByteBuffer GetZCLHeader(void) const;

private:
	/** */
	CZCLFrameControl frm_ctrl;
	/** legrand : 0x1021 */
	uint16_t manufacturer_code;
	/** */
	uint8_t transaction_number;
	/** */
	uint8_t cmd_id;
};

} // namespace NSEZSP
