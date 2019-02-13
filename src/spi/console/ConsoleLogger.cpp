/**
 * @file ConsoleLogger.cpp
 *
 * @brief Concrete implementation of a logger using the stdout and stderr output streams on the text console
 */

#include "ConsoleLogger.h"
#include <cstdarg>
#include <cstdio>

ConsoleErrorLogger::ConsoleErrorLogger() { }

ConsoleErrorLogger::~ConsoleErrorLogger() { }

void ConsoleErrorLogger::log(const char *format, ...) {

	va_list args;
	va_start(args, format);
	fprintf(stderr, format, args);
	va_end(args);
}

int ConsoleErrorLogger::overflow(int c) {
	if (c != EOF) {
		if (putchar(c) == EOF) {
			return EOF;
		}
	}
	return c;
}

static ConsoleErrorLogger consoleErrorLogger;

ConsoleLogger::ConsoleLogger(ILoggerError& errorLogger) : ILogger(errorLogger) {
}

ConsoleLogger::~ConsoleLogger() {
}

ConsoleLogger& ConsoleLogger::getInstance() {
	static ConsoleLogger instance(consoleErrorLogger); /* Unique instance of the singleton */

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

int ConsoleLogger::overflow(int c) {
	if (c != EOF) {
		//c = std::toupper(static_cast<char>(c),getloc());
		if (putchar(c) == EOF) {
			return EOF;
		}
	}
	return c;
}

std::ostream ILogger::loggerStream(&ConsoleLogger::getInstance());

