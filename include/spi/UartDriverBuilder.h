#ifndef __UARTDRIVDERBUILDER_H__
#define __UARTDRIVDERBUILDER_H__

#include <memory>
#include <functional>
#include "spi/IUartDriver.h"

namespace NSSPI {

typedef std::unique_ptr<IUartDriver, std::function<void(IUartDriver*)>> IUartDriverInstance;
class UartDriverBuilder
{
public:
	static IUartDriver *getInstance();
private:
	UartDriverBuilder() = default;
	static IUartDriverInstance mInstance;
};

} // namespace NSSPI

#endif
