/**
 * @file ember-gp-proxy-table-entry-struct.h
 *
 * @brief Represents one proxy table entry from Ember
 */

#pragma once

#include "ezsp/ezsp-protocol/ezsp-enum.h"
#include "ezsp/ezsp-protocol/struct/ember-gp-address-struct.h"
#include "spi/ByteBuffer.h"

namespace NSEZSP {

class CEmberGpProxyTableEntryStruct {
public:
	/**
	 * @brief Default constructor
	 *
	 * Construction without arguments is not allowed
	 */
	CEmberGpProxyTableEntryStruct() = delete;

	/**
	 * @brief Construction from a buffer
	 *
	 * @param raw_message The buffer to construct from
	 */
	explicit CEmberGpProxyTableEntryStruct(const NSSPI::ByteBuffer& raw_message);

	/**
	 * @brief Copy constructor
	 *
	 * Copy construction is forbidden on this class
	 */
	CEmberGpProxyTableEntryStruct(const CEmberGpProxyTableEntryStruct& other) = delete;

	/**
	 * @brief Assignment operator
	 *
	 * Assignment is forbidden on this class
	 */
	CEmberGpProxyTableEntryStruct& operator=(const CEmberGpProxyTableEntryStruct& other) = delete;

	/**
	 * @brief gpd address getter
	 *
	 * @return gpd address
	 */
	CEmberGpAddressStruct getGpdAddress() {
		return gpd;
	}

	/**
	 * @brief gpd status getter
	 *
	 * @return gp proxy table entry status
	 */
	EmberGpProxyTableEntryStatus getGpProxyTableEntryStatus() {
		return status;
	}


private:
	// EmberKeyData security_link_key; /*!< The link key to be used to secure this pairing link. */ -- WRONG SPEC
	EmberGpProxyTableEntryStatus status; /*!< Internal status of the proxy table entry. */
	uint32_t options; /*!< The tunneling options (this contains both options and extendedOptions from the spec). */
	CEmberGpAddressStruct gpd; /*!< The addressing info of the GPD. */
	EmberNodeId assigned_alias; /*!< The assigned alias for the GPD. */
	uint8_t security_options; /*!< The security options field. */
	EmberGpSecurityFrameCounter gpdSecurityFrameCounter; /*!< The security frame counter of the GPD. */
	EmberKeyData gpd_key; /*!< The key to use for GPD. */
	EmberGpSinkListEntry sink_list[GP_SINK_LIST_ENTRIES]; /*!< The list of sinks (hardcoded to 2 which is the spec minimum). */
	uint8_t groupcast_radius; /*!< The groupcast radius. */
	uint8_t search_counter; /*!< The search counter. */
};

} // namespace NSEZSP
