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

using NSSPI::UartDriver;

#ifdef USE_RARITAN
UartDriverBuilder::UartDriverBuilder(pp::Selector& selector) : eventSelector(selector) {
}

UartDriverBuilder::UartDriverBuilder() : UartDriverBuilder(*pp::SelectorSingleton::getInstance()) {
}
#else	// USE_RARITAN
UartDriverBuilder::UartDriverBuilder() = default;
#endif	// USE_RARITAN

std::unique_ptr<IUartDriver> UartDriverBuilder::create() const {
#ifdef USE_RARITAN
	/* TODO: When using a C++14 compliant compiler, the line below should be replaced with:
	 * return std::make_unique<NSSPI::RaritanUartDriver>(this->eventSelector);
	 */
	return std::unique_ptr<IUartDriver>(new NSSPI::RaritanUartDriver(this->eventSelector));
#else	// USE_RARITAN
	/* TODO: When using a C++14 compliant compiler, the line below should be replaced with:
	 * return std::make_unique<NSSPI::UartDriver>();
	 * Note: disabling code anaylis below because SonarCloud insists on blacklisting any dynamic allocation
	 * but we really require it is a factory or builder, and using it is mitigated by the unique_ptr container
	 */
	return std::unique_ptr<IUartDriver>(new NSSPI::UartDriver());	//NOSONAR
#endif	// USE_RARITAN
}
