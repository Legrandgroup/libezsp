#include <memory>

#include "spi/Logger.h"
#ifdef USE_RARITAN
#include "spi/raritan/RaritanLogger.h"
namespace NSSPI {
typedef RaritanLogger LoggerInstance;
}
#endif
#ifdef USE_CPPTHREADS
#include "spi/console/ConsoleLogger.h"
namespace NSSPI {
typedef ConsoleLogger LoggerInstance;
}
#endif


using NSSPI::Logger;
using NSSPI::ILogger;

NSSPI::ILoggerInstance Logger::mInstance;

ILogger *Logger::getInstance()
{
	static NSSPI::LoggerInstance logger;

	static bool static_init = []()->bool {
		mInstance = NSSPI::ILoggerInstance(&logger, [](ILogger* ptr)
        {
        });
		return true;
	}();
	return mInstance.get();
}
