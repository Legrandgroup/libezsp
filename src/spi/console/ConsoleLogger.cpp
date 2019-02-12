/**
 * @file ConsoleLogger.cpp
 *
 * @brief Concrete implementation of a logger using the stdout and stderr output streams on the text console
 */

#include "ConsoleLogger.h"
#include <cstdarg>
#include <cstdio>

ConsoleLogger::ConsoleLogger() {
}

ConsoleLogger::~ConsoleLogger() {
}

ConsoleLogger& ConsoleLogger::getInstance() {
	static ConsoleLogger instance; /* Unique instance of the singleton */

	return instance;
}

void ConsoleLogger::outputErrorLog(const char *format, ...) {

	va_list args;
	va_start(args, format);
	fprintf(stderr, format, args);
	va_end(args);
}

void ConsoleLogger::outputWarningLog(const char *format, ...) {

	va_list args;
	va_start(args, format);
	fprintf(stderr, format, args);
	va_end(args);
}

void ConsoleLogger::outputInfoLog(const char *format, ...) {

	va_list args;
	va_start(args, format);
	printf(format, args);
	va_end(args);
}

void ConsoleLogger::outputDebugLog(const char *format, ...) {

	va_list args;
	va_start(args, format);
	printf(format, args);
	va_end(args);
}

void ConsoleLogger::outputTraceLog(const char *format, ...) {

	va_list args;
	va_start(args, format);
	printf(format, args);
	va_end(args);
}
