#include "spi/raritan/RaritanUartDriver.h"
#include "spi/raritan/RaritanEventLoop.h"
#include "spi/raritan/RaritanUartInputObservable.h"

int main() {
	RaritanEventLoop eventLoop;
	RaritanUartInputObservable uartIncomingDataHandler;
	UartDriverRaritan uartDriver(eventLoop, uartIncomingDataHandler);

	uartDriver.open("/dev/ttyUSB0", 57600);
	eventLoop.run();

	return 0;
}
