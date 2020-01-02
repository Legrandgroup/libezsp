#ifndef __UARTDRIVDERBUILDER_H__
#define __UARTDRIVDERBUILDER_H__

#include <memory>
#include <functional>
#include "spi/IUartDriver.h"

typedef std::unique_ptr<IUartDriver, std::function<void(IUartDriver*)>> IUartDriverInstance;
class UartDriverBuilder
{
public:
	static IUartDriver *getInstance();
private:
	UartDriverBuilder() {}
	static IUartDriverInstance mInstance;
};

#endif
