/**
 * @file ezsp-dongle-observer.h
 *
 * @brief Describes the interface for an observer that wants to process incoming EZSP messages
 */

#pragma once

#include <vector>
#include "ezsp-protocol/ezsp-enum.h"

class CEzspDongleObserver {
public:
    CEzspDongleObserver() {};
    virtual ~CEzspDongleObserver() {};

    /**
     * @brief Method that will be invoked on dongle state changes
     *
     * @param i_state The new dongle state
     */
    virtual void handleDongleState( EDongleState i_state ) = 0;
    
    /**
     * @brief Method that will be invoked on incoming EZSP messages
     *
     * @param i_cmd The EZSP command
     * @param i_msg_receive The payload of the message
     */
    virtual void handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive ) = 0;

    /**
     * @brief Method that will be invoked when bootloader prompt is caught
     */
    virtual void handleBootloaderPrompt() = 0;

    /**
     * @brief Method that will be invoked when the bootloader is waiting for a firmware image transfer using X-modem
     */
    virtual void handleFirmwareXModemXfr() = 0;
};
