#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <memory>
#include "spi/ILogger.h"

class Logger
{
public:
	static ILogger& getInstance();
private:
	Logger();
	static ILogger* mInstance;
};
#endif
