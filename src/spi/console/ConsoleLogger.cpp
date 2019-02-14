/**
 * @file ConsoleLogger.cpp
 *
 * @brief Concrete implementation of a logger using the stdout and stderr output streams on the text console
 */

#include "ConsoleLogger.h"
#include <cstdarg>
#include <cstdio>

ConsoleStderrLogger::ConsoleStderrLogger(const LOG_LEVEL logLevel) :
		ILoggerStream(logLevel) { /* Set the parent classes' logger's level to what has been provided as constructor's argument */
}

void ConsoleStderrLogger::log(const char *format, ...) {

	va_list args;
	va_start(args, format);
	fprintf(stderr, format, args);
	va_end(args);
}

int ConsoleStderrLogger::overflow(int c) {
	if (c != EOF) {
		if (putchar(c) == EOF) {
			return EOF;
		}
	}
	return c;
}

ConsoleStdoutLogger::ConsoleStdoutLogger(const LOG_LEVEL logLevel) :
		ILoggerStream(logLevel) { /* Set the parent classes' logger's level to what has been provided as constructor's argument */
}

void ConsoleStdoutLogger::log(const char *format, ...) {

	va_list args;
	va_start(args, format);
	printf(format, args);
	va_end(args);
}

int ConsoleStdoutLogger::overflow(int c) {
	if (c != EOF) {
		if (putchar(c) == EOF) {
			return EOF;
		}
	}
	return c;
}

static ConsoleErrorLogger consoleErrorLogger;	/* Create a unique instance of the ConsoleErrorLogger that will be used to handle error logs */
static ConsoleDebugLogger consoleDebugLogger;	/* Create a unique instance of the ConsoleDebugLogger that will be used to handle debug logs */

ConsoleLogger::ConsoleLogger(ILoggerStream& errorLogger, ILoggerStream& debugLogger) :
		ILogger(errorLogger, debugLogger) {
}

ConsoleLogger& ConsoleLogger::getInstance() {
	static ConsoleLogger instance(consoleErrorLogger, consoleDebugLogger); /* Unique instance of the singleton */

	return instance;
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

void ConsoleLogger::outputTraceLog(const char *format, ...) {

	va_list args;
	va_start(args, format);
	printf(format, args);
	va_end(args);
}

/* Create unique (global) instances of each logger type, and store them inside the ILogger singleton */
std::ostream ILogger::loggerErrorStream(&ConsoleLogger::getInstance().errorLogger);
std::ostream ILogger::loggerDebugStream(&ConsoleLogger::getInstance().debugLogger);

