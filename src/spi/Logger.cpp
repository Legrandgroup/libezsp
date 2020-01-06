#include <memory>

#include "spi/Logger.h"
#ifdef USE_RARITAN
#include "spi/raritan/RaritanLogger.h"
#endif
#ifdef USE_CPPTHREADS
#include "spi/console/ConsoleLogger.h"
#endif

using NSSPI::Logger;
using NSSPI::ILogger;

std::unique_ptr<ILogger, std::function<void(ILogger*)>> Logger::mInstance;

ILogger& Logger::getInstance()
{
#ifdef USE_RARITAN
	static RaritanLogger logger;
#endif
#ifdef USE_CPPTHREADS
	static ConsoleLogger logger;
#endif

	static bool static_init = []()->bool {
		mInstance = std::unique_ptr<ILogger, std::function<void(ILogger*)>>(&logger, [](ILogger* ptr)
        {
        });
		return true;
	}();
	return *mInstance;
}
