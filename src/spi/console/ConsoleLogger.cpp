/**
 * @file ConsoleLogger.cpp
 *
 * @brief Concrete implementation of a logger using the stdout and stderr output streams on the text console
 */

#include "spi/Logger.h"
#include "ConsoleLogger.h"
#include <cstdarg>
#include <cstdio>

ConsoleStderrLogger::ConsoleStderrLogger(const LOG_LEVEL logLevel) :
		ILoggerStream(logLevel) { /* Set the parent classes' logger's level to what has been provided as constructor's argument */
}

ConsoleStderrLogger::~ConsoleStderrLogger() {
}

void ConsoleStderrLogger::log(const char *format, ...) {

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

/**
 * This method is a friend of ConsoleStderrLogger class
 * swap() is needed within operator=() to implement to copy and swap paradigm
 */
void swap(ConsoleStderrLogger& first, ConsoleStderrLogger& second) noexcept /* nothrow */ {
	using std::swap;	// Enable ADL

	swap(first.logLevel, second.logLevel);
	swap(first.enabled, second.enabled);
	swap(first.muted, second.muted);
	/* Once we have swapped the members of the two instances... the two instances have actually been swapped */
}

ConsoleStderrLogger& ConsoleStderrLogger::operator=(ConsoleStderrLogger other) {
	::swap(*this, other);
	return *this;
}

ConsoleStdoutLogger::ConsoleStdoutLogger(const LOG_LEVEL logLevel) :
		ILoggerStream(logLevel) { /* Set the parent classes' logger's level to what has been provided as constructor's argument */
}

ConsoleStdoutLogger::~ConsoleStdoutLogger() {
}

void ConsoleStdoutLogger::log(const char *format, ...) {

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

/**
 * This method is a friend of ConsoleStdoutLogger class
 * swap() is needed within operator=() to implement to copy and swap paradigm
 */
void swap(ConsoleStdoutLogger& first, ConsoleStdoutLogger& second) noexcept /* nothrow */ {
	using std::swap;	// Enable ADL

	swap(first.logLevel, second.logLevel);
	swap(first.enabled, second.enabled);
	swap(first.muted, second.muted);
	/* Once we have swapped the members of the two instances... the two instances have actually been swapped */
}

ConsoleStdoutLogger& ConsoleStdoutLogger::operator=(ConsoleStdoutLogger other) {
	::swap(*this, other);
	return *this;
}

static ConsoleErrorLogger consoleErrorLogger;	/* Create a unique instance of the ConsoleErrorLogger that will be used to handle error logs */
static ConsoleWarningLogger consoleWarningLogger;	/* Create a unique instance of the ConsoleWarningLogger that will be used to handle warning logs */
static ConsoleInfoLogger consoleInfoLogger;	/* Create a unique instance of the ConsoleInfoLogger that will be used to handle info logs */
static ConsoleDebugLogger consoleDebugLogger;	/* Create a unique instance of the ConsoleDebugLogger that will be used to handle debug logs */
static ConsoleTraceLogger consoleTraceLogger;	/* Create a unique instance of the ConsoleTraceLogger that will be used to handle trace logs */

ConsoleLogger::ConsoleLogger():
	ILogger(consoleErrorLogger, consoleWarningLogger, consoleInfoLogger, consoleDebugLogger, consoleTraceLogger)
{
}

ConsoleLogger::ConsoleLogger(ILoggerStream& errorLogger, ILoggerStream& warningLogger, ILoggerStream& infoLogger, ILoggerStream& debugLogger, ILoggerStream& traceLogger) :
		ILogger(errorLogger, warningLogger, infoLogger, debugLogger, traceLogger) {
}

ConsoleLogger::~ConsoleLogger() {
}

/* Create unique (global) instances of each logger type, and store them inside the ILogger (singleton)'s class static attribute */
std::ostream ILogger::loggerErrorStream(&Logger::getInstance().errorLogger);
std::ostream ILogger::loggerWarningStream(&Logger::getInstance().warningLogger);
std::ostream ILogger::loggerInfoStream(&Logger::getInstance().infoLogger);
std::ostream ILogger::loggerDebugStream(&Logger::getInstance().debugLogger);
std::ostream ILogger::loggerTraceStream(&Logger::getInstance().traceLogger);
