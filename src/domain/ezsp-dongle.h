#ifndef EZSP_DONGLE_H
#define EZSP_DONGLE_H

#include <string>
#include <iostream>
#include <vector>
#include <queue>

#include "ezsp-protocol/ezsp-enum.h"
#include "IUartDriver.h"
#include "ash.h"
#include "zigbee-message.h"
#include "out-zigbee-message.h"

typedef enum
{
    DONGLE_READY,
    DONGLE_REMOVE
}EDongleState;

class CDongleHandler
{
public:
    virtual ~CDongleHandler() {}
    virtual void dongleState( EDongleState i_state ) = 0;
    virtual void ashRxMessage( std::vector<uint8_t> i_message ) = 0;
    virtual void ezspHandler( EEzspCmd i_cmd, std::vector<uint8_t> i_message ) = 0;
};

/*
typedef struct sRspCb
{
    EEzspCmd i_cmd;
    std::function<void (EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive)> cb;
}SRspCb;
*/
typedef struct sMsg
{
    EEzspCmd i_cmd;
    std::vector<uint8_t> payload;
    std::function<void (EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive)> cb;
}SMsg;

class CEzspDongle : public IAsyncDataInputObserver, public CAshCallback
{
public:
    CEzspDongle( CDongleHandler *ipCb );
    CEzspDongle(const CEzspDongle&) = delete; /* No copy construction allowed (pointer data members) */
    ~CEzspDongle();

    CEzspDongle& operator=(const CEzspDongle&) = delete; /* No assignment allowed (pointer data members) */

    /**
     * Open connetion to dongle of type ezsp
     */
    bool open(IUartDriver *ipUart);


    /**
     * Send Ezsp Command
     */
    void sendCommand(EEzspCmd i_cmd, std::vector<uint8_t> i_cmd_payload = std::vector<uint8_t>(), 
                        std::function<void (EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive)> callBackFunction = nullptr );



    /**
     * callback de reception de l'uart
     */
    void handleInputData(const unsigned char* dataIn, const size_t dataLen);

    /**
     * callabck d'information de l'ash
     */
    void ashCbInfo( EAshInfo info );

private:
    IUartDriver *pUart;
    CAsh *ash;
    CDongleHandler *pHandler;
    GenericAsyncDataInputObservable uartIncomingDataHandler;
    std::queue<SMsg> sendingMsgQueue;
    bool wait_rsp;
    //std::vector<SRspCb> rspCbTable;

    void EzspProcess( std::vector<uint8_t> i_rx_msg );
    void sendNextMsg( void );

};

#endif // EZSP_DONGLE_H