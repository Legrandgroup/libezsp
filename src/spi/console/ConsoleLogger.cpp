/**
 * @file ConsoleLogger.cpp
 *
 * @brief Concrete implementation of a logger using the stdout and stderr output streams on the text console
 */

#include "spi/Logger.h"
#include "ConsoleLogger.h"
#include <cstdarg>

using NSSPI::ILogger;
using NSSPI::ConsoleStderrLogger;
using NSSPI::ConsoleStdoutLogger;
using NSSPI::ConsoleLogger;
using NSSPI::ConsoleErrorLogger;
using NSSPI::ConsoleWarningLogger;
using NSSPI::ConsoleInfoLogger;
using NSSPI::ConsoleDebugLogger;
using NSSPI::ConsoleTraceLogger;

ConsoleStderrLogger::ConsoleStderrLogger(const LOG_LEVEL newLogLevel) :
	ILoggerStream(newLogLevel) { /* Set the parent classes' logger's level to what has been provided as constructor's argument */
}

void ConsoleStderrLogger::logf(const char *format, ...) {

	if (this->enabled && !this->muted) {
		va_list args;
		va_start(args, format);
		fprintf(stderr, format, args);
		va_end(args);
	}
}

int ConsoleStderrLogger::overflow(int c) {
	if (c != EOF && this->enabled && !this->muted) {
		if (putchar(c) == EOF) {
			return EOF;
		}
	}
	return c;
}

ConsoleStdoutLogger::ConsoleStdoutLogger(const LOG_LEVEL newLogLevel) :
	ILoggerStream(newLogLevel) { /* Set the parent classes' logger's level to what has been provided as constructor's argument */
}

void ConsoleStdoutLogger::logf(const char *format, ...) {

	if (this->enabled && !this->muted) {
		va_list args;
		va_start(args, format);
		printf(format, args);
		va_end(args);
	}
}

int ConsoleStdoutLogger::overflow(int c) {
	if (c != EOF && this->enabled && !this->muted) {
		if (putchar(c) == EOF) {
			return EOF;
		}
	}
	return c;
}


static ConsoleErrorLogger consoleErrorLogger;	/* Create a unique instance of the ConsoleErrorLogger that will be used to handle error logs */
static ConsoleWarningLogger consoleWarningLogger;	/* Create a unique instance of the ConsoleWarningLogger that will be used to handle warning logs */
static ConsoleInfoLogger consoleInfoLogger;	/* Create a unique instance of the ConsoleInfoLogger that will be used to handle info logs */
static ConsoleDebugLogger consoleDebugLogger;	/* Create a unique instance of the ConsoleDebugLogger that will be used to handle debug logs */
static ConsoleTraceLogger consoleTraceLogger;	/* Create a unique instance of the ConsoleTraceLogger that will be used to handle trace logs */

ConsoleLogger::ConsoleLogger():
	ILogger(consoleErrorLogger, consoleWarningLogger, consoleInfoLogger, consoleDebugLogger, consoleTraceLogger) {
}

ConsoleLogger::ConsoleLogger(ILoggerStream& newErrorLogger, ILoggerStream& newWarningLogger, ILoggerStream& newInfoLogger, ILoggerStream& newDebugLogger, ILoggerStream& newTraceLogger) :
	ILogger(newErrorLogger, newWarningLogger, newInfoLogger, newDebugLogger, newTraceLogger) {
}

/* Create unique (global) instances of each logger type, and store them inside the ILogger (singleton)'s class static attribute */
std::ostream ILogger::loggerErrorStream(&Logger::getInstance()->errorLogger);
std::ostream ILogger::loggerWarningStream(&Logger::getInstance()->warningLogger);
std::ostream ILogger::loggerInfoStream(&Logger::getInstance()->infoLogger);
std::ostream ILogger::loggerDebugStream(&Logger::getInstance()->debugLogger);
std::ostream ILogger::loggerTraceStream(&Logger::getInstance()->traceLogger);
