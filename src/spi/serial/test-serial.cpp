#include "SerialUartDriver.h"
#include "CppThreadsTimerFactory.h"
#include "spi/GenericAsyncDataInputObservable.h"
#include "spi/GenericLogger.h"
#include <string>
#include <sstream>	// FIXME: for std::stringstream during debug
#include <iostream>	// FIXME: for std::cout during debug
#include <iomanip>	// FIXME: for std::hex during debug

/**
 * @brief Observer class for sending debugging output logs for incoming serial data
 */
class DebuggerDisplayer : public IAsyncDataInputObserver {
public:
	/**
	 * @brief Constructor
	 *
	 * @param displayName A prefix to use for each notification from the observable
	 */
	DebuggerDisplayer(const std::string& displayName): name(displayName) {};

	/**
	 * @brief Destructor
	 */
	~DebuggerDisplayer() {};

	/**
	 * @brief Handler invoked for each input data
	 *
	 * @param dataIn The pointer to the incoming bytes buffer
	 * @param dataLen The size of the data to read inside dataIn
	 */
	virtual void handleInputData(const unsigned char* dataIn, const size_t dataLen) {
		std::stringstream bufDump;

		for (size_t i =0; i<dataLen; i++) {
			bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(dataIn[i]) << " ";
		}
		clogI << this->name << ": Received buffer " << bufDump.str() << std::endl;
	};
private :
	std::string name;	/*!< The prefix "nickname" for this observer */
};

int main() {
	GenericAsyncDataInputObservable uartIncomingDataHandler;
	DebuggerDisplayer disp("Debugger displayer");
	uartIncomingDataHandler.registerObserver(&disp);

	SerialUartDriver uartDriver;
	uartDriver.setIncomingDataHandler(&uartIncomingDataHandler);

	CppThreadsTimerFactory timerFactory;
	ITimer *timer = timerFactory.create();
	timer->start(10000, [](ITimer* triggeringTimer) {
		clogI << "Timer finished (was launched by a " << triggeringTimer->duration << " ms timer)" << std::endl;
	});

	uartDriver.open("/dev/ttyUSB0", 57600);
	unsigned char buf[5] = { 0x1a, 0xc0, 0x38, 0xbc, 0x7e};
	size_t written;
	uartDriver.write(written, buf, 5);

	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	timer->stop();

	for(;;) { }

	return 0;
}
