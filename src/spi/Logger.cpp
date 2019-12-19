#ifndef __LOGGER_H__

#include "spi/Logger.h"
#ifdef USE_RARITAN
#include "spi/raritan/RaritanLogger.h"
#endif
#ifdef USE_CPPTHREADS
#include "spi/console/ConsoleLogger.h"
#endif

ILogger *Logger::mInstance;

ILogger& Logger::getInstance()
{

	static bool static_init = []()->bool {
#ifdef USE_RARITAN
		mInstance = new RaritanLogger();
#endif
#ifdef USE_CPPTHREADS
		mInstance = new ConsoleLogger();
#endif
		return true;
	}();
	return *mInstance;
}

#endif
