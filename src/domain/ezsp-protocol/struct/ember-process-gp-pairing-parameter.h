/**
 * @file ember-process-gp-pairing-parameter.h
 *
 * @brief Parameters for gpProxyTableProcessGpPairing (0xC9) command.
 *
 * Reference: docs-14-0563-16-batt-green-power-spec_ProxyBasic.pdf
 */
#pragma once

#include "../../byte-manip.h"
#include "../ezsp-enum.h"
#include "ember-gp-address-struct.h"
#include "ember-gp-sink-table-entry-struct.h"
#include "../../zbmessage/gp-pairing-command-option-struct.h"


#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

class CProcessGpPairingParam
{
    public:
        /**
         * @brief Default constructor
         *
         * Construction without arguments is not allowed
         */
        CProcessGpPairingParam() = delete;

        /**
         * @brief Copy constructor
         *
         * Copy construction is forbidden on this class
         */
        CProcessGpPairingParam(const CProcessGpPairingParam& other) = delete;

        /**
         * @brief constructor using sink table entry and extra parameter
         * 
         * @param i_sink_table_entry : sink table entry corresponding to gpd
         * @param i_add_sink : true to add, false to remove
         * @param i_remove_gpd : true to remove
         * @param i_sinkNetworkAddress : short network address of sink
         * @param i_sinkIeeeAddress : long network address of sink
         */
        CProcessGpPairingParam(CEmberGpSinkTableEntryStruct i_sink_table_entry, bool i_add_sink, bool i_remove_gpd, 
                                uint16_t i_sinkNetworkAddress, EmberEUI64 i_sinkIeeeAddress );


        /**
         * @brief Raw getter
         *
         * @return This object represented as a raw byte-buffer
         */
        std::vector<uint8_t> get() const;


    private:
        CGpPairingCommandOption options;        /*!< The options field of the GP Pairing command (see A.3.3.5.2 GP Pairing command for more details) */
        CEmberGpAddressStruct addr;     /*!< The target GPD */
        uint8_t commMode;       /*!< The communication mode of the GP Sink */
        uint16_t sinkNetworkAddress;    /*!< The network address of the GP Sink */
        uint16_t sinkGroupId;   /*!< The group ID of the GP Sink */
        uint16_t assignedAlias; /*!< The alias assigned to the GPD */
        EmberEUI64 sinkIeeeAddress;     /*!< The IEEE address of the GP Sink */
        EmberKeyData gpdKey;    /*!< The key to use for the target GPD */
        uint32_t gpdSecurityFrameCounter;       /*!< The gpd security frame counter */
        uint8_t forwardingRadius;       /*!< The forwarding radius */
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
