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

static ConsoleErrorLogger consoleErrorLogger;	/* Create a unique instance of the consoleErrorLogger that will be used to handle error logs */

ConsoleDebugLogger::ConsoleDebugLogger() { }

ConsoleDebugLogger::~ConsoleDebugLogger() { }

void ConsoleDebugLogger::log(const char *format, ...) {

	va_list args;
	va_start(args, format);
	printf(format, args);
	va_end(args);
}

int ConsoleDebugLogger::overflow(int c) {
	if (c != EOF) {
		if (putchar(c) == EOF) {
			return EOF;
		}
	}
	return c;
}

static ConsoleDebugLogger consoleDebugLogger;	/* Create a unique instance of the consoleErrorLogger that will be used to handle debug logs */

ConsoleLogger::ConsoleLogger(ILoggerError& errorLogger, ILoggerDebug& debugLogger) :
		ILogger(errorLogger, debugLogger) {
}

ConsoleLogger::~ConsoleLogger() {
}

ConsoleLogger& ConsoleLogger::getInstance() {
	static ConsoleLogger instance(consoleErrorLogger, consoleDebugLogger); /* Unique instance of the singleton */

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

/* Create unique (global) instances of each logger type, and store them inside the ILogger singleton */
std::ostream ILogger::loggerErrorStream(&ConsoleLogger::getInstance().errorLogger);
std::ostream ILogger::loggerDebugStream(&ConsoleLogger::getInstance().debugLogger);

