#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <queue>

#include "ezsp-protocol/ezsp-enum.h"
#include "spi/IUartDriver.h"
#include "ash.h"
#include "bootloader-prompt.h"
#include "ezsp-dongle-observer.h"
#include "spi/TimerBuilder.h"

extern "C" {	/* Avoid compiler warning on member initialization for structs (in -Weffc++ mode) */
    typedef struct sMsg
    {
        EEzspCmd i_cmd;
        std::vector<uint8_t> payload;
    }SMsg;
}

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

/**
 * @brief Requested state for the ezsp adapter
 */
enum class CEzspDongleMode {
    UNKNOWN,                            /*<! Unknown initial run mode for the dongle */
    EZSP_NCP,                           /*<! Dongle is in EZSP commands processor mode */
    BOOTLOADER_FIRMWARE_UPGRADE,        /*<! Dongle is in bootloader prompt mode, performing a firmware upgrade */
    BOOTLOADER_EXIT_TO_EZSP_NCP,        /*<! Dongle is in bootloader prompt mode, requested to switch back to EZSP_NCP mode */
};

class CEzspDongle : public IAsyncDataInputObserver, public CAshCallback
{
public:
    CEzspDongle( TimerBuilder &i_timer_factory, CEzspDongleObserver* ip_observer = nullptr );
	CEzspDongle() = delete; // Construction without arguments is not allowed
    CEzspDongle(const CEzspDongle&) = delete; /* No copy construction allowed (pointer data members) */
    ~CEzspDongle();

    CEzspDongle& operator=(CEzspDongle) = delete; /* No assignment allowed (pointer data members) */

    /**
     * @brief Open connetion to dongle of type ezsp
     */
    bool open(IUartDriver *ipUart);


    /**
     * @brief Send Ezsp Command
     */
    void sendCommand(EEzspCmd i_cmd, std::vector<uint8_t> i_cmd_payload = std::vector<uint8_t>() );



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
     * @brief Switch the EZSP adatper read/write behaviour to bootloader or EZSP/ASH mode
     *
     * @param requestedMode The new requested mode
     */
    void setMode(CEzspDongleMode requestedMode);

private:
    bool firstStartup;  /*!< Is this the first attempt to exchange with the dongle? If so, we will probe to check if the adapter is in EZSP or bootloader prompt mode */
    CEzspDongleMode lastKnownMode;    /*!< What is the current adapter mode (bootloader, EZSP/ASH mode etc.) */
    TimerBuilder &timer_factory;
    IUartDriver *pUart;
    CAsh *ash;
    CBootloaderPrompt *blp;
    GenericAsyncDataInputObservable uartIncomingDataHandler;
    std::queue<SMsg> sendingMsgQueue;
    bool wait_rsp;

    void sendNextMsg( void );

    /**
     * Notify Observer of this class
     */
    std::set<CEzspDongleObserver*> observers;
    void notifyObserversOfDongleState( EDongleState i_state );
    void notifyObserversOfEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_message );
    void notifyObserversOfBootloaderPrompt();
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
