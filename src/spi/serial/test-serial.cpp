#include "SerialUartDriver.h"
#include "SerialTimerFactory.h"
#include "../GenericAsyncDataInputObservable.h"
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
	GenericAsyncDataInputObservable uartIncomingDataHandler;
	DebuggerDisplayer disp("Debugger displayer");
	uartIncomingDataHandler.registerObserver(&disp);
	UartDriverSerial uartDriver(uartIncomingDataHandler);

	SerialTimerFactory serialTimerFactory;
	ITimer *serialTimer = serialTimerFactory.create();
	serialTimer->start(10000, [](ITimer* triggeringTimer) {
			std::cout << "Timer finished (was launched by a " << triggeringTimer->duration << " ms timer)" << std::endl;
	});

	uartDriver.open("/dev/ttyUSB0", 57600);

	unsigned char buf[5] = { 0x1a, 0xc0, 0x38, 0xbc, 0x7e};
	size_t written;
	uartDriver.write(written, buf, 5);

	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	serialTimer->stop();

	for(;;) { }

	return 0;
}
