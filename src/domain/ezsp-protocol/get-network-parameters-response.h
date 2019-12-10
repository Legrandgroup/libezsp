/**
 * @file get-network-parameters-response.h
 *
 * @brief Handles decoding of the current network parameters.
 */

#pragma once


#include "domain/ezsp-protocol/ezsp-enum.h"
#include "struct/ember-network-parameters.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

class CGetNetworkParamtersResponse
{
    public:
        /**
         * @brief Default constructor
         *
         * Construction without arguments is not allowed
         */
        CGetNetworkParamtersResponse() = delete;

        /**
         * @brief Construction from a buffer
         *
         * @param raw_message The buffer to construct from
         */
        CGetNetworkParamtersResponse(const std::vector<uint8_t>& raw_message);

        /**
         * @brief Copy constructor
         *
         * Copy construction is forbidden on this class
         */
        CGetNetworkParamtersResponse(const CGetNetworkParamtersResponse& other) = delete;

        /**
         * @brief Assignment operator
         *
         * Assignment is forbidden on this class
         */
        CGetNetworkParamtersResponse& operator=(const CGetNetworkParamtersResponse& other) = delete;

        /**
         * @brief An EmberStatus value indicating success or the reason for failure.
         */
        EEmberStatus getStatus() const { return status; }

        /**
         * @brief An EmberNodeType value indicating the current node type.
         */
        EmberNodeType getNodeType() const { return node_type; }

        /**
         * @brief The current network parameters.
         */
        CEmberNetworkParameters getParameters() const { return parameters; }

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
        friend std::ostream& operator<< (std::ostream& out, const CGetNetworkParamtersResponse& data);

    private:
        EEmberStatus status;
        EmberNodeType node_type;
        CEmberNetworkParameters parameters;
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
