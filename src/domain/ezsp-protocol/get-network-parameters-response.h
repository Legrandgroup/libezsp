/**
 * Returns the current network parameters.
 */
#pragma once


#include "ezsp-enum.h"
#include "struct/ember-network-parameters.h"

/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>

class CGetNetworkParamtersResponse
{
    public:
        CGetNetworkParamtersResponse(std::vector<uint8_t> raw_message);

        /**
         * An EmberStatus value indicating success or the reason for failure.
         */
        EEmberStatus getStatus(){ return status; }

        /**
         * An EmberNodeType value indicating the current node type.
         */
        EmberNodeType getNodeType(){ return node_type; }

        /**
         * The current network parameters.
         */
        CEmberNetworkParameters getParameters(){ return parameters; }

        /**
         * Retrieve a string for display
         */
        std::string String();

    private:
        EEmberStatus status;
        EmberNodeType node_type;
        CEmberNetworkParameters parameters;
};

#include <pp/official_api_end.h>
