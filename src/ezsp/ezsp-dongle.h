/**
 * @file ezsp-dongle.h
 *
 * @brief Handles EZSP communication with an adapter over a serial link
 */

#pragma once

#include <string>
#include <iostream>
#include <queue>
#include <set>

#include <ezsp/ezsp-protocol/ezsp-enum.h>
#include <ezsp/ezsp-adapter-version.h>
#include <spi/IUartDriver.h>
#include <spi/GenericAsyncDataInputObservable.h>
#include <spi/TimerBuilder.h>
#include <spi/IAsyncDataInputObserver.h>
#include <spi/ByteBuffer.h>

#include "ash-driver.h"
#include "bootloader-prompt-driver.h"
#include "ezsp/enum-generator.h"
#include "ezsp-dongle-observer.h"

extern "C" {	/* Avoid compiler warning on member initialization for structs (in -Weffc++ mode) */
	typedef struct {
		NSEZSP::EEzspCmd i_cmd;	/*!< The EZSP command to send */
		NSSPI::ByteBuffer payload;	/*!< The payload for the EZSP command (as a byte buffer) */
	} SMsg;
}
namespace NSEZSP {
	class CEzspDongle; // // Forward declaration for swap() below
}

void swap(NSEZSP::CEzspDongle& first, NSEZSP::CEzspDongle& second); /* Declaration before qualifying ::swap() as friend for class NSEZSP::CEzspDongle */

namespace NSEZSP {

#define EZSP_DONGLE_MODE_LIST(XX) \
    XX(UNKNOWN,=1)                          /*<! Unknown initial run mode for the dongle */ \
    XX(EZSP_NCP,)                           /*<! Dongle is in EZSP commands processor mode */ \
    XX(BOOTLOADER_FIRMWARE_UPGRADE,)        /*<! Dongle is in bootloader prompt mode, performing a firmware upgrade */ \
    XX(BOOTLOADER_EXIT_TO_EZSP_NCP,)        /*<! Dongle is in bootloader prompt mode, requested to switch back to EZSP_NCP mode */ \

class CEzspDongle : public NSSPI::IAsyncDataInputObserver, public CAshCallback {
public:
	/**
	 * @brief Requested mode for the EZSP adapter
	 *
	 * @note The lines above describes all states known in order to build both an enum and enum-to-string/string-to-enum methods
	 *       In this macro, XX is a placeholder for the macro to use for building.
	 *       We start numbering from 1, so that 0 can be understood as value not found for enum-generator.h
	 * @see enum-generator.h
	 */
	DECLARE_ENUM(Mode, EZSP_DONGLE_MODE_LIST);

	/**
	 * @brief Constructor
	 *
	 * @param[in] i_timer_builder A timer builder used to build timer objects
	 * @param[in] ip_observer An optional observer that will be notified when dongle state changes and when a EZSP message is received
	 *
	 * @note Observers can also be registered later on using method registerObserver()
	 */
	CEzspDongle(const NSSPI::TimerBuilder& i_timer_builder, CEzspDongleObserver* ip_observer = nullptr);

	/**
	 * @brief Default constructor
	 *
	 * @warning Construction without arguments is not allowed
	 */
	CEzspDongle() = delete;

	/**
	 * @brief Copy constructor
	 *
	 * @param other The object to copy from
	 */
	CEzspDongle(const CEzspDongle& other);

	/**
	 * @brief Destructor
	 */
	virtual ~CEzspDongle();

	/**
	 * @brief Set the serial port to use for communication with the EZSP adapter
	 *
	 * @param uartHandle A handle on a IUartDriver to send/receive data
	 */
	void setUart(NSSPI::IUartDriverHandle uartHandle);

	/**
	 * @brief Retrieve an observable to handle bytes received on the serial port
	 *
	 * @note This observable has been created at construction (see our constructor's uartHandler argument) and set as the observable for this uartHandler
	 * Thus, if another observable is setconfigured on uartHandler, the CEzsp instance will not receive incoming bytes anymore.
	 * To allow an external observer to attach to this observable and thus get the incoming serial bytes as well, we provide this utility
	 * method to expose the observable we created
	 *
	 * @return An observable instance that will notify its observers when new bytes are read from the serial port
	 */
	NSSPI::GenericAsyncDataInputObservable* getSerialReadObservable();

	/**
	 * @brief Reset and intialize an EZSP communication with the EZSP adapter
	 *
	 * @return true if the reset was successful, EZSP command can then be sent
	 */
	bool reset();

	/**
	 * @brief Set the current hardware & firmware XNCP version running on the EZSP adapter
	 *
	 * @param xncpManufacturerId The XNCP 16-bit manufacturer ID of the firmware running on the EZSP adapter
	 * @param xncpVersionNumber The XNCP 16-bit version number of the firmware running on the EZSP adapter
	 *
	 * @note Because our instance does not parse EZSP message, this method is invoked by external code, when an XNCP
	 *       INFO EZSP message is received, so that we can our own version details
	 * @warning Access to this method should be limited as much as possible, possibly even by design using an accesskey pattern
	 */
	void setFetchedXncpData(uint16_t xncpManufacturerId, uint16_t xncpVersionNumber);

	/**
	 * @brief Set the current EZSP stack version running on the EZSP adapter
	 *
	 * @param ezspStackVersion The EZSP stack version
	 *
	 * @note Because our instance does not parse EZSP message, this method is invoked by external code, when an XNCP
	 *       INFO EZSP message is received, so that we can our own version details
	 * @warning Access to this method should be limited as much as possible, possibly even by design using an accesskey pattern
	 */
	void setFetchedEzspVersionData(uint16_t ezspStackVersion);

	/**
	 * @brief Set the current EZSP version data running on the EZSP adapter
	 *
	 * @param ezspStackVersion The EZSP stack version
	 * @param ezspProtocolVersion The EZSP protocol version (EZSPv7, EZSPv8)
	 * @param ezspStackType The EZSP stack type
	 *
	 * @note Because our instance does not parse EZSP message, this method is invoked by external code, when an XNCP
	 *       INFO EZSP message is received, so that we can our own version details
	 * @warning Access to this method should be limited as much as possible, possibly even by design using an accesskey pattern
	 */
	void setFetchedEzspVersionData(uint16_t ezspStackVersion, uint8_t ezspProtocolVersion, uint8_t ezspStackType);

	/**
	 * @brief Get the current hardware & firmware, stack and protocol version running on the EZSP adapter
	 *
	 * @return The current version running on the EZSP adapter
	 */
	NSEZSP::EzspAdapterVersion getVersion() const;

	/**
	 * @brief Send an EZSP command to the EZSP adapter
	 *
	 * @param i_cmd The EZSP command to send
	 * @param i_cmd_payload The payload
	 */
	void sendCommand(EEzspCmd i_cmd, NSSPI::ByteBuffer i_cmd_payload = NSSPI::ByteBuffer() );

	/**
	 * @brief Callback invoked on EZSP received bytes
	 *
	 * @param[in] dataIn A buffer pointing to the received bytes
	 * @param[in] dataLen The number of bytes stored in @p dataIn
	 */
	void handleInputData(const unsigned char* dataIn, const size_t dataLen);

	/**
	 * @brief Callback invoked on ASH info
	 *
	 * @param info The new ASH state
	 */
	void ashCbInfo(AshCodec::EAshInfo info);

	/**
	 * Managing Observer of this class
	 */
	bool registerObserver(CEzspDongleObserver* observer);
	bool unregisterObserver(CEzspDongleObserver* observer);

	/**
	 * @brief Makes initialization timeout trigger a switch to firmware upgrade mode
	 *
	 * Default behaviour for initialization timeouts is to probe the bootloader prompt and if found, to run the EZSP application
	 * in the hope the adapter will move back to EZSP mode
	 */
	void forceFirmwareUpgradeOnInitTimeout();

	/**
	 * @brief Switch the EZSP adatper read/write behaviour to bootloader or EZSP/ASH mode
	 *
	 * @param requestedMode The new requested mode
	 */
	void setMode(CEzspDongle::Mode requestedMode);

	/**
	 * \brief swap function to allow implementing of copy-and-swap idiom on instances of CEzspDongle
	 *
	 * This function will swap all attributes of \p first and \p second
	 * See http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
	 *
	 * \param first The first object
	 * \param second The second object
	 */
	friend void (::swap)(NSEZSP::CEzspDongle& first, NSEZSP::CEzspDongle& second);

	/**
	 * \brief Assignment operator
	 * \param other The object to assign to the lhs
	 *
	 * \return The object that has been assigned the value of \p other
	 */
	CEzspDongle& operator=(const CEzspDongle other);

private:
	bool firstStartup;  /*!< Is this the first attempt to exchange with the dongle? If so, we will probe to check if the adapter is in EZSP or bootloader prompt mode */
	NSEZSP::EzspAdapterVersion version; /*!< The version details of this EZSP adapter (firmware and hardware) */
	CEzspDongle::Mode lastKnownMode;    /*!< What is the current adapter mode (bootloader, EZSP/ASH mode etc.) */
	bool switchToFirmwareUpgradeOnInitTimeout;   /*!< Shall we directly move to firmware upgrade if we get an ASH timeout, if not, we will run the application (default behaviour) */
	const NSSPI::TimerBuilder* timerBuilder;    /*!< A timer builder used to generate timers */
	NSSPI::IUartDriverHandle uartHandle; /*!< A reference to the IUartDriver object used to send/receive serial data to the EZSP adapter */
	NSSPI::GenericAsyncDataInputObservable uartIncomingDataHandler; /*!< The observable handler that will dispatch received incoming bytes to observers */
	uint8_t ezspSeqNum;	/*!< The EZSP sequence number (wrapping 0-255 counter) */
	NSEZSP::AshDriver ash;   /*!< An ASH encoder/decoder instance */
	NSEZSP::BootloaderPromptDriver blp;  /*!< A bootloader prompt decoder instance */
	std::queue<SMsg> sendingMsgQueue;	/*!< The EZSP messages queued to be sent to the adapter */
	std::mutex sendingMsgQueueMutex;	/*!< A mutex protecting access to attribute sendingMsgQueue */
	bool wait_rsp;	/*!< Are we currently waiting for an EZSP response to an EZSP command we have sent? */
	std::set<CEzspDongleObserver*> observers;	/*!< List of observers of this instance */
	std::mutex ezspWriteMutex;	/*!< Mutex allowing exclusive writes to the EZSP adapter */

	/**
	 * @brief Send the next message in our EZSP message queue (sendingMsgQueue)
	 */
	void sendNextMsg();

	/**
	 * @brief Notify all observers of this instance that the dongle state has changed
	 *
	 * @param i_state The new dongle state
	 */
	void notifyObserversOfDongleState( EDongleState i_state );

	/**
	 * @brief Notify all observers of this instance of a newly incoming EZSP message
	 *
	 * @param i_cmd The EZSP command received
	 * @param i_message The payload of the EZSP command
	 */
	void notifyObserversOfEzspRxMessage( EEzspCmd i_cmd, NSSPI::ByteBuffer i_message );

	/**
	 * @brief Notify all observers of this instance that the dongle is running the booloader and that a bootloader prompt has been detected
	 */
	void notifyObserversOfBootloaderPrompt();

	/**
	 * @brief Notify all observers of this instance that the dongle is waiting for a firmware image transfer using X-modem for a firmware update
	 */
	void notifyObserversOfFirmwareXModemXfrReady();

	/**
	 * CEzspDongleObserver handle functions on 'this' self
	 */
	void handleDongleState( EDongleState i_state );
	void handleResponse( EEzspCmd i_cmd );

protected:
	/**
	 * @brief Check if we know which EZSP protocol version is used by the EZSP adapter
	 *
	 * @return true if we know the EZSP protocol version
	 */
	bool knownEzspProtocolVersion() const;

	/**
	 * @brief Check if the EZSP protocol version used by the EZSP adapter is greater or equal to a minimum value
	 *
	 * @param minIncludedVersion Minimum acceptable version (version should be greater or equal to that value)
	 *
	 * @return true if the EZSP protocol version matches the requirement
	 */
	bool knownEzspProtocolVersionGE(uint8_t minIncludedVersion) const;

	/**
	 * @brief Check if the EZSP protocol version used by the EZSP adapter is strictly lower than a maximum value
	 *
	 * @param maxExcludedVersion Maximum acceptable version (version should be strictly lower than that value, not equal)
	 *
	 * @return true if the EZSP protocol version matches the requirement
	 */
	bool knownEzspProtocolVersionLT(uint8_t maxExcludedVersion) const;
};

} // namespace NSEZSP
