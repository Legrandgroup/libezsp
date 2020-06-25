/**
 * @file green-power-frame.h
 *
 * @brief Handles decoding of a green power frame
 */
#pragma once

#include <cstdint>

#include <ezsp/export.h>
#include <ezsp/ezsp-protocol/ezsp-enum.h>
#include <ezsp/zbmessage/zcl.h>
#include <spi/ByteBuffer.h>

namespace NSEZSP {

typedef std::array<uint8_t, 13> GPNonce;

typedef enum {
	GPD_NO_SECURITY                      =       0x0,
	GPD_FRM_COUNTER_MIC_SECURITY         =       0x2,
	GPD_ENCRYPT_FRM_COUNTER_MIC_SECURITY =       0x3
} EGpSecurityLevel;

typedef enum {
	GPD_KEY_TYPE_NO_KEY                         =       0x0,
	GPD_KEY_TYPE_ZB_NWK_KEY                     =       0x1,
	GPD_KEY_TYPE_GPD_GROUP_KEY                  =       0x2,
	GPD_KEY_TYPE_NWK_KEY_DERIVED_GPD_GROUP_KEY  =       0x3,
	GPD_KEY_TYPE_OOB_KEY                        =       0x4,
	GPD_KEY_TYPE_DERIVED_INDIVIDUAL_KEY         =       0x7,
} EGpSecurityKeyType;

class LIBEXPORT CGpFrame {
public:
	/**
	 * @brief Default constructor
	 *
	 * Construction without arguments is not allowed
	 */
	CGpFrame();

	/**
	 * @brief Assignment operator
	 *
	 * Copy construction is forbidden on this class
	 */
	CGpFrame& operator=(const CGpFrame& other) = default;

	/**
	 * @brief Construction from an incoming ezsp raw message
	 *
	 * @param raw_message The buffer to construct from
	 */
	explicit CGpFrame(const NSSPI::ByteBuffer& raw_message);

	/**
	 * @brief Dump this instance as a string
	 *
	 * @return The resulting string
	 */
	std::string String() const;

	/**
	 * @brief Serialize to an iostream
	 *
	 * @param out The original output stream
	 * @param data The object to serialize
	 *
	 * @return The new output stream with serialized data appended
	 */
	friend std::ostream& operator<< (std::ostream& out, const CGpFrame& data) {
		out << data.String();
		return out;
	}

	// getter
	uint8_t getApplicationId() const {
		return application_id;
	}
	uint8_t getLinkValue() const {
		return link_value;
	}
	uint8_t getSequenceNumber() const {
		return sequence_number;
	}
	uint32_t getSourceId() const {
		return source_id;
	}
	EGpSecurityLevel getSecurity() const {
		return security;
	}
	EGpSecurityKeyType getKeyType() const {
		return key_type;
	}
	bool isAutoCommissioning() const {
		return auto_commissioning;
	}
	bool isRxAfterTx() const {
		return rx_after_tx;
	}
	uint32_t getSecurityFrameCounter() const {
		return security_frame_counter;
	}
	uint8_t getCommandId() const {
		return command_id;
	}
	uint32_t getMic() const {
		return mic;
	}
	uint8_t getProxyTableEntry() const {
		return proxy_table_entry;
	}
	NSSPI::ByteBuffer getPayload() const {
		return payload;
	}

	/**
	 * @brief Generate the network FC byte corresponding to this frame
	 *
	 * This byte should be equivalent to the FC byte in the transmitted packet that lead to the EZSP message used as the base to construct this CGpFrame
	 *
	 * @return The network FC byte
	 */
	uint8_t toNwkFCByteField() const;

	/**
	 * @brief Generate the extended network FC byte corresponding to this frame
	 *
	 * This byte should be equivalent to the FC byte in the transmitted packet that lead to the EZSP message used as the base to construct this CGpFrame
	 *
	 * @return The extended network FC byte
	 */
	uint8_t toExtNwkFCByteField() const;

	/**
	 * @brief Validate the MIC in this frame against the provided key
	 *
	 * @param[in] i_gpd_key The key to authenticate the message
	 *
	 * @return true if the MIC in this frame validates the full GP frame using @p i_gpd_key as an authentication key
	 */
	bool validateMIC(const NSEZSP::EmberKeyData& i_gpd_key) const;

private:
	/**
	 * @brief Computes a nonce according to the GP specs
	 *
	 * @param sourceId The source ID used as input to compute the nonce
	 * @param frameCounter The security frame counter used as input to compute the nonce
	 *
	 * @return A GPNonce object
	 */
	static NSEZSP::GPNonce computeNonce(uint32_t sourceId, uint32_t frameCounter);

	/**
	 * @brief Run the EAS CBC multiple times on a group of AES blocks
	 * @param[in] i_gpd_key is the 128-bit AES key
	 * @param[in] X0 is the initial input of AES-CBC
	 * @param[in] B0 is the very first block
	 * @param[in] B The input buffer on which to run AES CBC
	 * @param[out] lastIndex An indicative value of how many times the AES-CBC cipher was run (this is the index i of the Xi returned)
	 *
	 * @return Xi, the last result of AES-CBC
	 *
	 * B is split in multiple AES block size long buffers that will be sequentially used as input to the AES-CBC cipher
	 * @note B should be a multiple of AES block size (16 bytes) or an error will occur and an empty buffer will be returned
	 */
	static NSSPI::ByteBuffer getLastXiAESCBC(const EmberKeyData& i_gpd_key, const NSSPI::ByteBuffer& X0, const NSSPI::ByteBuffer& B0, const NSSPI::ByteBuffer& B, unsigned int& lastIndex);

	uint8_t application_id;
	uint8_t link_value;
	uint8_t sequence_number;
	uint32_t source_id;
	EGpSecurityLevel security;
	EGpSecurityKeyType key_type;
	bool auto_commissioning;
	bool rx_after_tx;
	uint32_t security_frame_counter;
	uint8_t command_id;
	uint32_t mic;
	uint8_t proxy_table_entry;
	NSSPI::ByteBuffer payload;
};

} // namespace NSEZSP
