/**
 * @file apsoption.h
 *
 * @brief Handles encoding/decoding of the 16-bit APS option word
 */

#pragma once

#include <cstdint>

namespace NSEZSP {

class CAPSOption; /* Forward declaration */
void swap(CAPSOption& first, CAPSOption& second) noexcept; /* Declaration before qualifying ::swap() as friend for class CAPSOption */

class CAPSOption {
public:
	/**
	 * @brief Default constructor
	 */
	CAPSOption();

	// concatenate ember
	uint16_t GetEmberApsOption(void) const;
	void SetEmberApsOption( const uint16_t i_option );

	// flags
	/** Include the destination EUI64 in the network frame. */
	bool dest_ieee;
	/** Send a ZDO request to discover the node ID of the destination, if it is not already know. */
	bool enable_addr_discovery;
	/** Causes a route discovery to be initiated if no route to the destination is known. */
	bool enable_route_discovery;
	/** Send the message using APS Encryption, using the Link Key shared with the destination node to encrypt the data at the APS Level. */
	bool encryption;
	/** Causes a route discovery to be initiated even if one is known. */
	bool force_route_discovery;
	/** This message is part of a fragmented message. This
	  * option may only be set for unicasts. The groupId field
	  * gives the index of this fragment in the low-order byte. If
	  * the low-order byte is zero this is the first fragment and
	  * the high-order byte contains the number of fragments in
	  * the message.
	*/
	bool fragment;
	/** Resend the message using the APS retry mechanism. */
	bool retry;
	/** Include the source EUI64 in the network frame. */
	bool src_ieee;
	/** This incoming message is a ZDO request not handled by
	  * the EmberZNet stack, and the application is responsible
	  * for sending a ZDO response. This flag is used only when
	  * the ZDO is configured to have requests handled by the
	  * application. See the
	  * EZSP_CONFIG_APPLICATION_ZDO_FLAGS configuration
	  * parameter for more information.
	*/
	bool zdo_rsp_requiered;
};

} // namespace NSEZSP
