#include "spi/raritan/RaritanUARTDriver.h"
#include "spi/raritan/RaritanEventLoop.h"

int main() {
	RaritanEventLoop eventLoop;
	UartDriverRaritan uartDriver(eventLoop);

	uartDriver.open("/dev/ttyUSB0", 57600);
	eventLoop.run();

	return 0;
}
