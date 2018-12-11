/**
 * A structure containing a key and its associated data.
 */
#pragma once

#include "../ezsp-enum.h"

/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>

class CEmberKeyStruct
{
    public:
        CEmberKeyStruct(std::vector<uint8_t> raw_message){ setRaw(raw_message); }

        // little endian encoded
        void setRaw(std::vector<uint8_t> raw_message);

        /**
         * A bitmask indicating the presence of data within the
         * various fields in the structure.
         */
        EmberKeyStructBitmask getBitmask(){ return bitmask; }

        /**
         * The type of the key.
         */
        EmberKeyType getType(){ return type; }
            
        /**
         * The actual key data.
         */
        EmberKeyData getKey(){ return key; }

        /**
         * The outgoing frame counter associated with the key.
         */
        uint32_t getOutgoingFrameCounter(){ return outgoingFrameCounter; }

        /**
         * The frame counter of the partner device associated
         * with the key.
         */
        uint32_t getIncomingFrameCounter(){ return incomingFrameCounter; }

        /**
         * The sequence number associated with the key.
         */
        uint8_t getSequenceNumber(){ return sequenceNumber; }

        /**
         * The IEEE address of the partner device also in
         * possession of the key.
         */
        EmberEUI64 getPartnerEUI64(){ return partnerEUI64; }

        /**
         * For display
         */
        std::string String();

    private:
        EmberKeyStructBitmask bitmask;
        EmberKeyType type;
        EmberKeyData key;
        uint32_t outgoingFrameCounter;
        uint32_t incomingFrameCounter;
        uint8_t sequenceNumber;
        EmberEUI64 partnerEUI64;
};

#include <pp/official_api_end.h>
