#include "spi/raritan/RaritanUartDriver.h"
#include "spi/raritan/RaritanEventLoop.h"
#include "spi/GenericAsyncDataInputObservable.h"
#include <string>
#include <sstream>	// FIXME: for std::stringstream during debug
#include <iostream>	// FIXME: for std::cout during debug
#include <iomanip>	// FIXME: for std::hex during debug

class DebuggerDisplayer : public IAsyncDataInputObserver {
public:
	DebuggerDisplayer(const std::string& displayName): name(displayName) {};
	~DebuggerDisplayer() {};

	virtual void handleInputData(const unsigned char* dataIn, const size_t dataLen) {
		std::stringstream bufDump;

		for (size_t i =0; i<dataLen; i++) {
			bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(dataIn[i]) << " ";
		}
		std::cout << this->name << ": Received buffer " << bufDump.str() << std::endl;
	};
private :
	std::string name;
};

int main() {
	RaritanEventLoop eventLoop;
	GenericAsyncDataInputObservable uartIncomingDataHandler;
	DebuggerDisplayer disp("Debugger displayer");
	uartIncomingDataHandler.registerObserver(&disp);
	UartDriverRaritan uartDriver(eventLoop, uartIncomingDataHandler);

	uartDriver.open("/dev/ttyUSB0", 57600);
	eventLoop.run();

	return 0;
}
