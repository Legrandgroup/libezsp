#ifndef __UARTDRIVDERBUILDER_H__
#define __UARTDRIVDERBUILDER_H__

#include <memory>
#include "spi/IUartDriver.h"

class UartDriverBuilder
{
public:
	static IUartDriver *getUartDriver();
private:
	UartDriverBuilder() {}
};

#endif
