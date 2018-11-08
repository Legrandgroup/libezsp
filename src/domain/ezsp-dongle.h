#ifndef EZSP_DONGLE_H
#define EZSP_DONGLE_H

#include <string>

#include "IUartDriver.h"
#include "ash.h"


class CEzspDongle : public IAsyncDataInputObserver, public CAshCallback
{
public:
    CEzspDongle();

    /**
     * Open connetion to dongle of type ezsp
     */
    bool open(IUartDriver *ipUart);






    /**
     * callback de reception de l'uart
     */
    virtual void handleInputData(const unsigned char* dataIn, const size_t dataLen);

    /**
     * callabck d'information de l'ash
     */
    virtual void ashCbInfo( EAshInfo info ) { cout <<  "ashCbInfo : " << info << endl; };

private:
    IUartDriver *pUart;
    CAsh ash(this);

};

#endif // EZSP_DONGLE_H