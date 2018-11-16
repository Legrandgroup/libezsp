/**
 * A structure containing a child node's data.
 */
#pragma once

#include "../ezsp-enum.h"

class CEmberChildDataStruct
{
    public:
        CEmberChildDataStruct(std::vector<uint8_t> raw_message){ setRaw(raw_message); }

        // little endian encoded
        void setRaw(std::vector<uint8_t> raw_message);

        /**
         * The EUI64 of the child
         */
        EmberEUI64 getEui64(){ return eui64; }

        /**
         * The node type of the child
         */
        EmberNodeType getType() { return type; }

        /**
         * The short address of the child
         */
        EmberNodeId getId(){ return id; }

        /**
         * The phy of the child
         */
        uint8_t getPhy(){ return phy; }

        /**
         * The power of the child
         */
        uint8_t getPower(){ return power; }

        /**
         * The timeout of the child
         */
        uint8_t getTimeout() { return timeout; }

        /**
         * The GPD's EUI64.
         */
        EmberEUI64 getGpdIeeeAddress(){ return gpdIeeeAddress; }

        /**
         * The GPD's source ID.
         */
        uint32_t getSourceId() { return sourceId; }

        /**
         * The GPD Application ID.
         */
        uint8_t getApplicationId(){ return applicationId; }

        /**
         * The GPD endpoint.
         */
        uint8_t getEndpoint(){ return endpoint; }

        /**
         * For display
         */
        std::string String();
        /*
        friend std::ostream& operator<< (std::ostream& out, const CEmberChildDataStruct& data){
            out << data.String();
            return out;
        }
        */

    private:
        EmberEUI64 eui64; // The EUI64 of the child
        EmberNodeType type; // The node type of the child
        EmberNodeId id; // The short address of the child
        uint8_t phy; // The phy of the child
        uint8_t power; // The power of the child
        uint8_t timeout; // The timeout of the child
        EmberEUI64 gpdIeeeAddress; // The GPD's EUI64.
        uint32_t sourceId; // The GPD's source ID.
        uint8_t applicationId; // The GPD Application ID.
        uint8_t endpoint; // The GPD endpoint.
};