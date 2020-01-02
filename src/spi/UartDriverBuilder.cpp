#include "spi/UartDriverBuilder.h"

//#define DYNAMIC_ALLOCATION

#ifdef USE_RARITAN
#include "spi/raritan/RaritanUartDriver.h"
typedef RaritanUartDriver UartDriver;
#endif
#ifdef USE_SERIALCPP
#include "spi/serial/SerialUartDriver.h"
typedef SerialUartDriver UartDriver;
#endif
#ifdef USE_MOCKSERIAL
#include "spi/mock-uart/MockUartDriver.h"
typedef MockUartDriver UartDriver;
#endif


IUartDriverInstance UartDriverBuilder::mInstance;

IUartDriver *UartDriverBuilder::getInstance()
{
#ifndef DYNAMIC_ALLOCATION
	static UartDriver uartDriver;

	static bool static_init = []()->bool {
		mInstance = IUartDriverInstance(&uartDriver, [](IUartDriver* ptr)
        {
        });
		return true;
	}();
	return mInstance.get();
#else //DYNAMIC_ALLOCATION
	return new UartDriver();
#endif
}
