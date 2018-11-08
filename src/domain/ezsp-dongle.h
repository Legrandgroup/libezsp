#ifndef EZSP_DONGLE_H
#define EZSP_DONGLE_H

#include <string>

#include "IUartDriver.h"
#include "ash.h"

typedef enum
{
    FLOW_CTRL_NONE,
    FLOW_CTRL_SOFTWARE, // xon/xoff
    FLOW_CTRL_HARDWARE  // ctr/dts
}EFlowControl;

class CEzspDongle : public IAsyncDataInputObserver, public CAshCallback
{
public:
    CEzspDongle();

    /**
     * Open connetion to dongle of type ezsp
     */
    bool open(const std::string& serialPortName, unsigned int baudRate, EFlowControl flowControl);



    virtual void handleInputData(const unsigned char* dataIn, const size_t dataLen) {
		std::stringstream bufDump;

		for (size_t i =0; i<dataLen; i++) {
			bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(dataIn[i]) << " ";
		}
		cout << name << ": Received buffer " << bufDump.str() << endl;
	};

    void ashCbInfo( EAshInfo info ) {
        cout <<  "ashCbInfo : " << info << endl;
    };

private:
    IUartDriver *uart;
    CAsh ash(this);

};

#endif // EZSP_DONGLE_H