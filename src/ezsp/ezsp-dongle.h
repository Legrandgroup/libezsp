#pragma once

#include <string>
#include <iostream>
#include <queue>
#include <set>

#include <ezsp/ezsp-protocol/ezsp-enum.h>
#include "spi/IUartDriver.h"
#include "ash.h"
#include "bootloader-prompt.h"
#include "ezsp-dongle-observer.h"
#include "spi/TimerBuilder.h"
#include "spi/IAsyncDataInputObserver.h"
#include "spi/GenericAsyncDataInputObservable.h"
#include "spi/ByteBuffer.h"

extern "C" {	/* Avoid compiler warning on member initialization for structs (in -Weffc++ mode) */
    typedef struct sMsg
    {
        NSEZSP::EEzspCmd i_cmd;
        NSSPI::ByteBuffer payload;
    }SMsg;
}

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN
namespace NSEZSP {

/**
 * @brief Requested state for the ezsp adapter
 */
enum class CEzspDongleMode {
    UNKNOWN,                            /*<! Unknown initial run mode for the dongle */
    EZSP_NCP,                           /*<! Dongle is in EZSP commands processor mode */
    BOOTLOADER_FIRMWARE_UPGRADE,        /*<! Dongle is in bootloader prompt mode, performing a firmware upgrade */
    BOOTLOADER_EXIT_TO_EZSP_NCP,        /*<! Dongle is in bootloader prompt mode, requested to switch back to EZSP_NCP mode */
};

class CEzspDongle : public NSSPI::IAsyncDataInputObserver, public CAshCallback
{
public:
    CEzspDongle( NSSPI::TimerBuilder &i_timer_factory, CEzspDongleObserver* ip_observer = nullptr );
	CEzspDongle() = delete; // Construction without arguments is not allowed
    CEzspDongle(const CEzspDongle&) = delete; /* No copy construction allowed (pointer data members) */
    virtual ~CEzspDongle() = default;

    CEzspDongle& operator=(CEzspDongle) = delete; /* No assignment allowed (pointer data members) */

    /**
     * @brief Open connetion to dongle of type ezsp
     */
    bool open(NSSPI::IUartDriver *ipUart);


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
     */
    void ashCbInfo( EAshInfo info );

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
    void setMode(CEzspDongleMode requestedMode);

private:
    bool firstStartup;  /*!< Is this the first attempt to exchange with the dongle? If so, we will probe to check if the adapter is in EZSP or bootloader prompt mode */
    CEzspDongleMode lastKnownMode;    /*!< What is the current adapter mode (bootloader, EZSP/ASH mode etc.) */
    bool switchToFirmwareUpgradeOnInitTimeout;   /*!< Shall we directly move to firmware upgrade if we get an ASH timeout, if not, we will run the application (default behaviour) */
    NSSPI::TimerBuilder &timer_factory;
    NSSPI::IUartDriver *pUart;
    CAsh ash;
    CBootloaderPrompt blp;
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
#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
