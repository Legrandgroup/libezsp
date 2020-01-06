#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <memory>
#include <functional>
#include "spi/ILogger.h"

namespace NSSPI {

class Logger
{
public:
	static ILogger& getInstance();
private:
	Logger() = default;
	static std::unique_ptr<ILogger, std::function<void(ILogger*)>> mInstance;
};

} // namespace NSSPI

#endif
