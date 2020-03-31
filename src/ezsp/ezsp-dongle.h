#pragma once

#include <string>
#include <iostream>
#include <queue>
#include <set>

#include <ezsp/ezsp-protocol/ezsp-enum.h>
#include "spi/IUartDriver.h"
#include "ash-driver.h"
#include "bootloader-prompt.h"
#include "ezsp-dongle-observer.h"
#include <spi/GenericAsyncDataInputObservable.h>
#include "spi/TimerBuilder.h"
#include "spi/IAsyncDataInputObserver.h"
#include "spi/ByteBuffer.h"
#include "ezsp/enum-generator.h"

extern "C" {	/* Avoid compiler warning on member initialization for structs (in -Weffc++ mode) */
    typedef struct {
        NSEZSP::EEzspCmd i_cmd;
        NSSPI::ByteBuffer payload;
    } SMsg;
}

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

    CEzspDongle(const NSSPI::TimerBuilder& i_timer_builder, CEzspDongleObserver* ip_observer = nullptr);
	CEzspDongle() = delete; // Construction without arguments is not allowed
    CEzspDongle(const CEzspDongle&) = delete; /* No copy construction allowed (pointer data members) */
    virtual ~CEzspDongle() = default;

    CEzspDongle& operator=(CEzspDongle) = delete; /* No assignment allowed (pointer data members) */

    /**
     * @brief Set the serial port to use for communication with the EZSP adapter
     * 
     * @param uartHandle A handle on a IUartDriver to send/receive data
     */
    void setUart(NSSPI::IUartDriverHandle uartHandle);

    /**
     * @brief Reset and intialize an EZSP communication with the EZSP adapter
     * 
     * @return true if the reset was successful, EZSP command can then be sent
     */
    bool reset();

    /**
     * @brief Send Ezsp Command
     */
    void sendCommand(EEzspCmd i_cmd, NSSPI::ByteBuffer i_cmd_payload = NSSPI::ByteBuffer() );

    /**
     * @brief Callback invoked on UART received bytes
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

private:
    bool firstStartup;  /*!< Is this the first attempt to exchange with the dongle? If so, we will probe to check if the adapter is in EZSP or bootloader prompt mode */
    CEzspDongle::Mode lastKnownMode;    /*!< What is the current adapter mode (bootloader, EZSP/ASH mode etc.) */
    bool switchToFirmwareUpgradeOnInitTimeout;   /*!< Shall we directly move to firmware upgrade if we get an ASH timeout, if not, we will run the application (default behaviour) */
    const NSSPI::TimerBuilder& timerBuilder;    /*!< A timer builder used to generate timers */
    NSSPI::IUartDriverHandle uartHandle; /*!< A reference to the IUartDriver object used to send/receive serial data to the EZSP adapter */
	NSEZSP::AshDriver ash;   /*!< An ASH encoder/decoder instance */
    NSEZSP::CBootloaderPrompt blp;  /*!< A bootloader prompt decoder instance */
    NSSPI::GenericAsyncDataInputObservable uartIncomingDataHandler;
    std::queue<SMsg> sendingMsgQueue;
    bool wait_rsp;
    std::set<CEzspDongleObserver*> observers;   /*!< List of observers of this instance */

    void sendNextMsg( void );

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
};

} // namespace NSEZSP
