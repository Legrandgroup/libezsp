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

void ConsoleLogger::outputErrorLog(const char *format, ...) {

	va_list args;
	va_start(args, format);
	PPD_ERR(format, args);
	va_end(args);
}

void ConsoleLogger::outputWarningLog(const char *format, ...) {

	va_list args;
	va_start(args, format);
	PPD_WARN(format, args);
	va_end(args);
}

void ConsoleLogger::outputInfoLog(const char *format, ...) {

	va_list args;
	va_start(args, format);
	PPD_INFO(format, args);
	va_end(args);
}

void ConsoleLogger::outputDebugLog(const char *format, ...) {

	va_list args;
	va_start(args, format);
	PPD_DEBUG(format, args);
	va_end(args);
}

void ConsoleLogger::outputTraceLog(const char *format, ...) {

	va_list args;
	va_start(args, format);
	PPD_TRACE(format, args);
	va_end(args);
}
