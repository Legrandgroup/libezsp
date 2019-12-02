/**
 * @file ember-gp-proxy-table-entry-struct.h
 *
 * @brief Represents one proxy table entry from Ember
 */

#pragma once

#include "../ezsp-enum.h"
#include "ember-gp-address-struct.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN


class CEmberGpProxyTableEntryStruct
{
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
        CEmberGpProxyTableEntryStruct(const std::vector<uint8_t>& raw_message);

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
        CEmberGpAddressStruct getGpdAddress(){ return gpd; }


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

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
