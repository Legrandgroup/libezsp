/**
 * Network parameters.
 */
#pragma once

#include "../ezsp-enum.h"

class CEmberNetworkParameters
{
    public:
        CEmberNetworkParameters() : extend_pan_id(0), pan_id(0), radio_tx_power(3), 
            radio_channel(11), join_method(EMBER_USE_MAC_ASSOCIATION), nwk_manager_id(0), nwk_update_id(0), channels(0) {;}

        // little endian encoded
        void setRaw(std::vector<uint8_t> raw_message);
        std::vector<uint8_t> getRaw();

        /**
         * The network's extended PAN identifier.
         */
        uint64_t getExtendPanId(){ return extend_pan_id; }
        void setExtendPanId(uint64_t i_extend_pan_id){ extend_pan_id = i_extend_pan_id; }

        /**
         * The network's PAN identifier.
         */
        uint16_t getPanId(){ return pan_id; }
        void setPanId(uint16_t i_pan_id){ pan_id = i_pan_id; }

        /**
         * A power setting, in dBm.
         */
        uint8_t getRadioTxPower(){ return radio_tx_power; }
        void setRadioTxPower(uint8_t i_radio_tx_power){ radio_tx_power = i_radio_tx_power; }

        /**
         * A radio channel.
         */
        uint8_t getRadioChannel(){ return radio_channel; }
        void setRadioChannel(uint8_t i_radio_channel){ radio_channel = i_radio_channel; }

        /**
         * The method used to initially join the network.
         */
        EmberJoinMethod getJoinMethod(){ return join_method; }
        void setJoinMethod(EmberJoinMethod i_join_method){ join_method = i_join_method; }

        /**
         * NWK Manager ID. The ID of the network manager in
         * the current network. This may only be set at joining
         * when using EMBER_USE_NWK_COMMISSIONING
         * as the join method.
         */
        EmberNodeId getNwkManagerId(){ return nwk_manager_id; }
        void setNwkManagerId(EmberNodeId i_nwk_manager_id){ nwk_manager_id = i_nwk_manager_id; }

        /**
         * NWK Update ID. The value of the ZigBee
         * nwkUpdateId known by the stack. This is used to
         * determine the newest instance of the network after a
         * PAN ID or channel change. This may only be set at
         * joining when using
         * EMBER_USE_NWK_COMMISSIONING as the join
         * method.
         */
        uint8_t getNwkUpdateId(){ return nwk_update_id; }
        void setNwkUpdateId(uint8_t i_nwk_update_id){ nwk_update_id = i_nwk_update_id; }

        /**
         * NWK channel mask. The list of preferred channels
         * that the NWK manager has told this device to use
         * when searching for the network. This may only be
         * set at joining when using
         * EMBER_USE_NWK_COMMISSIONING as the join
         * method.
         */
        uint32_t getChannels(){ return channels; }
        void setChannels(uint32_t i_channels){ channels = i_channels; }

        /**
         * For display
         */
        std::string String();

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

