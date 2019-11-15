/**
 * paramters for gpProxyTableProcessGpPairing (0xC9) command.
 * reference is for docs-14-0563-16-batt-green-power-spec_ProxyBasic.pdf
 */
#pragma once

#include "../../byte-manip.h"
#include "../ezsp-enum.h"
#include "ember-gp-address-struct.h"


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


    private:
        /**
         * The options field of the GP Pairing command.
         * see A.3.3.5.2 GP Pairing command for more details
         */
        uint32_t options;
        /**
         * The target GPD.
         */
        CEmberGpAddressStruct addr;
        /**
         * The communication mode of the GP Sink.
         */
        uint8_t commMode;
        /**
         * The network address of the GP Sink.
         */
        uint16_t sinkNetworkAddress;
        /**
         * The group ID of the GP Sink.
         */
        uint16_t sinkGroupId;
        /**
         * The alias assigned to the GPD.
         */
        uint16_t assignedAlias;
        /**
         * The IEEE address of the GP Sink.
         */
        EmberEUI64 sinkIeeeAddress;
        /**
         * The key to use for the target GPD.
         */
        EmberKeyData gpdKey;
        /**
         * The gpd security frame counter.
         */
        uint32_t gpdSecurityFrameCounter;
        /**
         * The forwarding radius.
         */
        uint8_t forwardingRadius;
};