/**
 * @file RaritanLogger.cpp
 *
 * @brief Concrete implementation of a logger using Raritan's framework
 */

#include "RaritanLogger.h"
#include <pp/diag.h>

RaritanLogger::RaritanLogger() {
}

RaritanLogger::~RaritanLogger() {
}

void RaritanLogger::log(const ILogger::LOG_LEVEL log_level, const char *format, ...) {

	va_list args;

	va_start(args, format);
	switch (log_level) {
	case ILogger::LOG_LEVEL::ERROR:
		PPD_ERR(format, args);
		break;
	case ILogger::LOG_LEVEL::WARNING:
		PPD_WARN(format, args);
		break;
	case ILogger::LOG_LEVEL::INFO:
		PPD_INFO(format, args);
		break;
	case ILogger::LOG_LEVEL::DEBUG:
		PPD_DEBUG(format, args);
		break;
	case ILogger::LOG_LEVEL::TRACE:
		PPD_TRACE(format, args);
		break;
	}
	va_end(args);
}
