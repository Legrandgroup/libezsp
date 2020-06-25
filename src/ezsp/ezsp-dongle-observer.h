/**
 * @file ezsp-dongle-observer.h
 *
 * @brief Describes the interface for an observer that wants to process incoming EZSP messages
 */

#pragma once

#include <ezsp/ezsp-protocol/ezsp-enum.h>
#include "spi/ByteBuffer.h"

namespace NSEZSP {

class CEzspDongleObserver {
public:
	CEzspDongleObserver() = default;
	virtual ~CEzspDongleObserver() = default;

	/**
	 * @brief Method that will be invoked on dongle state changes
	 *
	 * @param i_state The new dongle state
	 */
	virtual void handleDongleState( EDongleState i_state ) { /* Default implementation does nothing, add your own handler here in derived observer classes */ }

	/**
	 * @brief Method that will be invoked on incoming EZSP messages
	 *
	 * @param i_cmd The EZSP command
	 * @param i_msg_receive The payload of the message
	 */
	virtual void handleEzspRxMessage( EEzspCmd i_cmd, NSSPI::ByteBuffer i_msg_receive ) { /* Default implementation does nothing, add your own handler here in derived observer classes */ }

	/**
	 * @brief Method that will be invoked when bootloader prompt is caught
	 */
	virtual void handleBootloaderPrompt() { /* Default implementation does nothing, add your own handler here in derived observer classes */ }

	/**
	 * @brief Method that will be invoked when the bootloader is waiting for a firmware image transfer using X-modem
	 */
	virtual void handleFirmwareXModemXfr() { /* Default implementation does nothing, add your own handler here in derived observer classes */ }
};

} // namespace NSEZSP
