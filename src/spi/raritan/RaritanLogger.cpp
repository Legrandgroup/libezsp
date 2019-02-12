/**
 * @file RaritanLogger.cpp
 *
 * @brief Concrete implementation of a logger using Raritan's framework
 */

#include "RaritanLogger.h"
#include <pp/diag.h>
#include <cstdarg>

RaritanLogger::RaritanLogger() {
}

RaritanLogger::~RaritanLogger() {
}

RaritanLogger& RaritanLogger::getInstance() {
	static ConsoleLogger instance; /* Unique instance of the singleton */

	return instance;
}

void RaritanLogger::outputErrorLog(const char *format, ...) {

	va_list args;
	va_start(args, format);
	PPD_ERR(format, args);
	va_end(args);
}

void RaritanLogger::outputWarningLog(const char *format, ...) {

	va_list args;
	va_start(args, format);
	PPD_WARN(format, args);
	va_end(args);
}

void RaritanLogger::outputInfoLog(const char *format, ...) {

	va_list args;
	va_start(args, format);
	PPD_INFO(format, args);
	va_end(args);
}

void RaritanLogger::outputDebugLog(const char *format, ...) {

	va_list args;
	va_start(args, format);
	PPD_DEBUG(format, args);
	va_end(args);
}

void RaritanLogger::outputTraceLog(const char *format, ...) {

	va_list args;
	va_start(args, format);
	PPD_TRACE(format, args);
	va_end(args);
}
