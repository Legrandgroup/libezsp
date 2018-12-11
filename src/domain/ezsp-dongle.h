#ifndef EZSP_DONGLE_H
#define EZSP_DONGLE_H

#include <string>
#include <iostream>
#include <vector>
#include <queue>

#include "ezsp-protocol/ezsp-enum.h"
#include "../spi/IUartDriver.h"
#include "ash.h"
#include "ezsp-dongle-observer.h"
#include "../spi/ITimerFactory.h"


typedef struct sMsg
{
    EEzspCmd i_cmd;
    std::vector<uint8_t> payload;
}SMsg;

class CEzspDongle : public IAsyncDataInputObserver, public CAshCallback
{
public:
    CEzspDongle( ITimerFactory &i_timer_factory, CEzspDongleObserver* ip_observer = nullptr );
	CEzspDongle() = delete; // Construction without arguments is not allowed
    CEzspDongle(const CEzspDongleObserver&) = delete; /* No copy construction allowed (pointer data members) */
    ~CEzspDongle();

    CEzspDongle& operator=(const CEzspDongleObserver&) = delete; /* No assignment allowed (pointer data members) */

    /**
     * Open connetion to dongle of type ezsp
     */
    bool open(IUartDriver *ipUart);


    /**
     * Send Ezsp Command
     */
    void sendCommand(EEzspCmd i_cmd, std::vector<uint8_t> i_cmd_payload = std::vector<uint8_t>() );



    /**
     * callback de reception de l'uart
     */
    void handleInputData(const unsigned char* dataIn, const size_t dataLen);

    /**
     * callabck d'information de l'ash
     */
    void ashCbInfo( EAshInfo info );

    /**
     * Managing Observer of this class
     */
	bool registerObserver(CEzspDongleObserver* observer);
	bool unregisterObserver(CEzspDongleObserver* observer); 

private:
    ITimerFactory &timer_factory;
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

#endif // EZSP_DONGLE_H