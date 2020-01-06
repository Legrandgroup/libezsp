#include "spi/UartDriverBuilder.h"

//#define DYNAMIC_ALLOCATION

using NSSPI::UartDriverBuilder;
using NSSPI::IUartDriver;
using NSSPI::IUartDriverInstance;

#ifdef USE_RARITAN
#include "spi/raritan/RaritanUartDriver.h"
namespace NSSPI {
typedef RaritanUartDriver UartDriver;
}
#endif
#ifdef USE_SERIALCPP
#include "spi/serial/SerialUartDriver.h"
namespace NSSPI {
typedef SerialUartDriver UartDriver;
}
#endif
#ifdef USE_MOCKSERIAL
#include "spi/mock-uart/MockUartDriver.h"
namespace NSSPI {
typedef MockUartDriver UartDriver;
}
#endif
using NSSPI::UartDriver;


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
