/**
 * @file Logger.h
 * 
 * @brief Singleton logger
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <memory>
#include <functional>

namespace NSSPI {
class ILogger;

typedef std::unique_ptr<ILogger, std::function<void(ILogger*)>> ILoggerInstance;

class Logger
{
public:
	static ILogger *getInstance();
private:
	Logger() = default;
	static ILoggerInstance mInstance;
};

} // namespace NSSPI

#include "spi/ILogger.h"

#endif
