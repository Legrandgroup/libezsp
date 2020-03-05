#include "spi/UartDriverBuilder.h"

#undef __UARTDRIVER_SPI_FOUND__
#ifdef USE_RARITAN
#define __UARTDRIVER_SPI_FOUND__
#include "spi/raritan/RaritanUartDriver.h"
namespace NSSPI {
typedef RaritanUartDriver UartDriver;
}
#endif
#ifdef USE_SERIALCPP
# ifdef __UARTDRIVER_SPI_FOUND__
#  error Duplicate UART driver SPI in use
# endif
#define __UARTDRIVER_SPI_FOUND__
#include "spi/serial/SerialUartDriver.h"
namespace NSSPI {
typedef SerialUartDriver UartDriver;
}
#endif
#ifdef USE_MOCKSERIAL
# ifdef __UARTDRIVER_SPI_FOUND__
#  error Duplicate UART driver SPI in use
# endif
#define __UARTDRIVER_SPI_FOUND__
#include "spi/mock-uart/MockUartDriver.h"
namespace NSSPI {
typedef MockUartDriver UartDriver;
}
#endif
#ifndef __UARTDRIVER_SPI_FOUND__
# error At least one UART driver SPI should be selected
#endif
#undef __UARTDRIVER_SPI_FOUND__

//#define DYNAMIC_ALLOCATION

using NSSPI::UartDriverBuilder;
using NSSPI::IUartDriver;
using NSSPI::IUartDriverInstance;

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
