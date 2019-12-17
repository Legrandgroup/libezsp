#include "spi/UartDriverBuilder.h"

#ifdef USE_RARITAN
#include "spi/raritan/RaritanUartDriver.h"
#endif
#ifdef USE_SERIALCPP
#include "spi/serial/SerialUartDriver.h"
#endif
#ifdef USE_MOCKSERIAL
#include "spi/mock-uart/MockUartDriver.h"
#endif

IUartDriver *UartDriverBuilder::getUartDriver()
{
#ifdef USE_RARITAN
	return new RaritanUartDriver();
#endif
#ifdef USE_SERIALCPP
	return new SerialUartDriver();
#endif
#ifdef USE_MOCKSERIAL
	return new MockUartDriver();
#endif
}
