#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <queue>

#include "ezsp-protocol/ezsp-enum.h"
#include "spi/IUartDriver.h"
#include "ash.h"
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
     * @brief Switch dongle read/write behaviour to bootloader or EZSP/ASH mode
     *
     * @param dongleInBootloaderMode If true, the dongle will consider that the adapter is in bootloader prompt mode, if false, it will consider that the adapter is in EZSP/ASH command mode
     */
    void setBootloaderMode(bool dongleInBootloaderMode);

private:
    bool bootloaderMode;    /*!< Is the adapter in bootloader prompt mode? If false, we are in applicative EZSP/ASH mode */
    TimerBuilder &timer_factory;
    IUartDriver *pUart;
    CAsh *ash;
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
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
