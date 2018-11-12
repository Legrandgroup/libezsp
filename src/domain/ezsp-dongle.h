#ifndef EZSP_DONGLE_H
#define EZSP_DONGLE_H

#include <string>
#include <iostream>

#include "ezsp-enum.h"
#include "IUartDriver.h"
#include "ash.h"
#include "zigbee-message.h"
#include "out-zigbee-message.h"

class CEzspHandler
{
public:
    virtual ~CEzspHandler() {}
    virtual void ashRxMessage( std::vector<uint8_t> i_message ) = 0;
    virtual void ezspHandler( EEzspCmd i_cmd, std::vector<uint8_t> i_message ) = 0;
};


typedef struct sRspCb
{
    EEzspCmd i_cmd;
    std::function<void (EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive)> cb;
}SRspCb;

class CEzspDongle : public IAsyncDataInputObserver, public CAshCallback
{
public:
    CEzspDongle( CEzspHandler *ipCb );
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
    virtual void handleInputData(const unsigned char* dataIn, const size_t dataLen);

    /**
     * callabck d'information de l'ash
     */
    virtual void ashCbInfo( EAshInfo info ) { std::cout <<  "ashCbInfo : " << info << std::endl; };

private:
    IUartDriver *pUart;
    CAsh *ash;
    CEzspHandler *pHandler;
    CEzspHandler *pCb;

    std::vector<SRspCb> rspCbTable;

    void EzspProcess( std::vector<uint8_t> i_rx_msg );

};

#endif // EZSP_DONGLE_H