/**
 * @file RaritanLogger.cpp
 *
 * @brief Concrete implementation of a logger using Raritan's framework
 */

#include "RaritanLogger.h"
#include <pp/diag.h>
#include <cstdarg>
using NSSPI::ILogger;
using NSSPI::RaritanGenericLogger;
using NSSPI::RaritanErrorLogger;
using NSSPI::RaritanWarningLogger;
using NSSPI::RaritanInfoLogger;
using NSSPI::RaritanDebugLogger;
using NSSPI::RaritanLogger;

RaritanGenericLogger::RaritanGenericLogger(const LOG_LEVEL newLogLevel) :
		ILoggerStream(newLogLevel),
		m_buffer()	/* TODO: pre-allocate the buffer to a default size to avoid reallocs on the fly */
{ /* Set the parent classes' logger's level to what has been provided as constructor's argument */
}

RaritanGenericLogger::~RaritanGenericLogger() {
}

int RaritanGenericLogger::overflow(int c) {
	if (c != EOF) {
		if (c == '\n') {
			this->logf(this->m_buffer.c_str());
			this->m_buffer = "";
		}
		else {
			if (c>0xff)
				return EOF;
			this->m_buffer += static_cast<char>(c & 0xff);
		}
	}
	return c;
}

/**
 * This method is a friend of RaritanErrorLogger class
 * swap() is needed within operator=() to implement to copy and swap paradigm
 */
void NSSPI::swap(RaritanErrorLogger& first, RaritanErrorLogger& second) noexcept /* nothrow */ {
	using std::swap;	// Enable ADL

	swap(first.logLevel, second.logLevel);
	swap(first.enabled, second.enabled);
	swap(first.muted, second.muted);
	/* Once we have swapped the members of the two instances... the two instances have actually been swapped */
}

RaritanErrorLogger& RaritanErrorLogger::operator=(RaritanErrorLogger other) {
	NSSPI::swap(*this, other);
	return *this;
}

void RaritanErrorLogger::logf(const char *format, ...) {
	if (this->enabled && !this->muted) {
		va_list args;
		va_start(args, format);
		PPD_ERR(format, args);
		va_end(args);
	}
}

/**
 * This method is a friend of RaritanWarningLogger class
 * swap() is needed within operator=() to implement to copy and swap paradigm
 */
void NSSPI::swap(RaritanWarningLogger& first, RaritanWarningLogger& second) /* nothrow */ {
	using std::swap;	// Enable ADL

	swap(first.logLevel, second.logLevel);
	swap(first.enabled, second.enabled);
	swap(first.muted, second.muted);
	/* Once we have swapped the members of the two instances... the two instances have actually been swapped */
}

RaritanWarningLogger& RaritanWarningLogger::operator=(RaritanWarningLogger other) {
	NSSPI::swap(*this, other);
	return *this;
}

void RaritanWarningLogger::logf(const char *format, ...) {
	if (this->enabled && !this->muted) {
		va_list args;
		va_start(args, format);
		PPD_WARN(format, args);
		va_end(args);
	}
}

/**
 * This method is a friend of RaritanInfoLogger class
 * swap() is needed within operator=() to implement to copy and swap paradigm
 */
void swap(RaritanInfoLogger& first, RaritanInfoLogger& second) noexcept /* nothrow */ {
	using std::swap;	// Enable ADL

	swap(first.logLevel, second.logLevel);
	swap(first.enabled, second.enabled);
	swap(first.muted, second.muted);
	/* Once we have swapped the members of the two instances... the two instances have actually been swapped */
}

RaritanInfoLogger& RaritanInfoLogger::operator=(RaritanInfoLogger other) {
	::swap(*this, other);
	return *this;
}

void RaritanInfoLogger::logf(const char *format, ...) {
	if (this->enabled && !this->muted) {
		va_list args;
		va_start(args, format);
		PPD_INFO(format, args);
		va_end(args);
	}
}

/**
 * This method is a friend of RaritanDebugLogger class
 * swap() is needed within operator=() to implement to copy and swap paradigm
 */
void swap(RaritanDebugLogger& first, RaritanDebugLogger& second) /* nothrow */ {
	using std::swap;	// Enable ADL

	swap(first.logLevel, second.logLevel);
	swap(first.enabled, second.enabled);
	swap(first.muted, second.muted);
	/* Once we have swapped the members of the two instances... the two instances have actually been swapped */
}

RaritanDebugLogger& RaritanDebugLogger::operator=(RaritanDebugLogger other) {
	::swap(*this, other);
	return *this;
}

void RaritanDebugLogger::logf(const char *format, ...) {
	if (this->enabled && !this->muted) {
		va_list args;
		va_start(args, format);
		PPD_DEBUG(format, args);
		va_end(args);
	}
}

static RaritanErrorLogger raritanErrorLogger;	/* Create a unique instance of the RaritanErrorLogger that will be used to handle error logs */
static RaritanWarningLogger raritanWarningLogger;	/* Create a unique instance of the RaritanWarningLogger that will be used to handle warning logs */
static RaritanInfoLogger raritanInfoLogger;	/* Create a unique instance of the RaritanInfoLogger that will be used to handle info logs */
static RaritanDebugLogger raritanDebugLogger;	/* Create a unique instance of the RaritanDebugLogger that will be used to handle debug logs */
//static RaritanTraceLogger raritanTraceLogger;	/* Create a unique instance of the RaritanTraceLogger that will be used to handle trace logs */

RaritanLogger::RaritanLogger():
	ILogger(raritanErrorLogger, raritanWarningLogger, raritanInfoLogger, raritanDebugLogger, raritanDebugLogger)
{
}

RaritanLogger::RaritanLogger(ILoggerStream& usedErrorLogger, ILoggerStream& usedWarningLogger, ILoggerStream& usedInfoLogger, ILoggerStream& usedDebugLogger, ILoggerStream& usedTraceLogger) :
		ILogger(usedErrorLogger, usedWarningLogger, usedInfoLogger, usedDebugLogger, usedTraceLogger) {
}

/* Create unique (global) instances of each logger type, and store them inside the ILogger (singleton)'s class static attribute */
std::ostream ILogger::loggerErrorStream(&RaritanLogger::getInstance().errorLogger);
std::ostream ILogger::loggerWarningStream(&RaritanLogger::getInstance().warningLogger);
std::ostream ILogger::loggerInfoStream(&RaritanLogger::getInstance().infoLogger);
std::ostream ILogger::loggerDebugStream(&RaritanLogger::getInstance().debugLogger);
std::ostream ILogger::loggerTraceStream(&RaritanLogger::getInstance().traceLogger);

/* TODO: convert the old TRACE methods below into ILoggerStream methods and remove all RaritanLogger::* methods below... today TRACE level messages are handled as DEBUG level messages */
//void RaritanLogger::outputTraceLog(const char *format, ...) {
//
//	va_list args;
//	va_start(args, format);
//	PPD_TRACE(format, args);
//	va_end(args);
//}
