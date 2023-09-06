/**
 * @file ember-network-parameters.h
 *
 * @brief Ember network parameters encoder as used as payload for EZSP command 0x1E (formNetwork)
 */

#pragma once

#include <string>
#include "ezsp/ezsp-protocol/ezsp-enum.h"
#include "spi/ByteBuffer.h"

namespace NSEZSP {

class CEmberNetworkParameters; /* Forward declaration */
void swap(CEmberNetworkParameters& first, CEmberNetworkParameters& second) noexcept; /* Declaration before qualifying ::swap() as friend for class CEmberNetworkParameters */

class CEmberNetworkParameters {
public:
	/**
	 * @brief Default constructor
	 *
	 * Construction without arguments is not allowed
	 */
	CEmberNetworkParameters();

	/**
	 * @brief Construction from a buffer
	 *
	 * @param raw_message The buffer to construct from
	 * @param skip The number of leading bytes to skip in buffer @p raw_message
	 */
	CEmberNetworkParameters(const NSSPI::ByteBuffer& raw_message, const std::string::size_type skip = 0);

	NSSPI::ByteBuffer getRaw() const;

	/**
	 * @brief The network's extended PAN identifier.
	 */
	uint64_t getExtendPanId() const {
		return extend_pan_id;
	}
	void setExtendPanId(const uint64_t i_extend_pan_id) {
		extend_pan_id = i_extend_pan_id;
	}

	/**
	 * @brief The network's PAN identifier.
	 */
	uint16_t getPanId() const {
		return pan_id;
	}
	void setPanId(const uint16_t i_pan_id) {
		pan_id = i_pan_id;
	}

	/**
	 * @brief A power setting, in dBm.
	 */
	uint8_t getRadioTxPower() const {
		return radio_tx_power;
	}
	void setRadioTxPower(const uint8_t i_radio_tx_power) {
		radio_tx_power = i_radio_tx_power;
	}

	/**
	 * @brief A radio channel.
	 */
	uint8_t getRadioChannel() const {
		return radio_channel;
	}
	void setRadioChannel(const uint8_t i_radio_channel) {
		radio_channel = i_radio_channel;
	}

	/**
	 * @brief The method used to initially join the network.
	 */
	EmberJoinMethod getJoinMethod() const {
		return join_method;
	}
	void setJoinMethod(const EmberJoinMethod i_join_method) {
		join_method = i_join_method;
	}

	/**
	 * @brief NWK Manager ID
	 *
	 * The ID of the network manager in
	 * the current network. This may only be set at joining
	 * when using EMBER_USE_NWK_COMMISSIONING
	 * as the join method.
	 */
	EmberNodeId getNwkManagerId() const {
		return nwk_manager_id;
	}
	void setNwkManagerId(const EmberNodeId i_nwk_manager_id) {
		nwk_manager_id = i_nwk_manager_id;
	}

	/**
	 * @brief Get NWK Update ID.
	 *
	 * The value of the ZigBee
	 * nwkUpdateId known by the stack. This is used to
	 * determine the newest instance of the network after a
	 * PAN ID or channel change. This may only be set at
	 * joining when using
	 * EMBER_USE_NWK_COMMISSIONING as the join
	 * method.
	 */
	uint8_t getNwkUpdateId() const {
		return nwk_update_id;
	}
	void setNwkUpdateId(const uint8_t i_nwk_update_id) {
		nwk_update_id = i_nwk_update_id;
	}

	/**
	 * @brief Get NWK channel mask.
	 *
	 * The list of preferred channels
	 * that the NWK manager has told this device to use
	 * when searching for the network. This may only be
	 * set at joining when using
	 * EMBER_USE_NWK_COMMISSIONING as the join
	 * method.
	 */
	uint32_t getChannels() const {
		return channels;
	}
	void setChannels(const uint32_t i_channels) {
		channels = i_channels;
	}

	/**
	 * @brief Dump this instance as a string
	 *
	 * @return The resulting string
	 */
	std::string toString() const;

	/**
	 * @brief Serialize to an iostream
	 *
	 * @param out The original output stream
	 * @param data The object to serialize
	 *
	 * @return The new output stream with serialized data appended
	 */
	friend std::ostream& operator<< (std::ostream& out, const CEmberNetworkParameters& data) {
		out << data.toString();
		return out;
	}

private:
	uint64_t extend_pan_id;
	uint16_t pan_id;
	uint8_t radio_tx_power;
	uint8_t radio_channel;
	EmberJoinMethod join_method;
	EmberNodeId nwk_manager_id;
	uint8_t nwk_update_id;
	uint32_t channels;
};

} // namespace NSEZSP
