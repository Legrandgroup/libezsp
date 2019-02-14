/**
 * @file RaritanLogger.cpp
 *
 * @brief Concrete implementation of a logger using Raritan's framework
 */

#include "RaritanLogger.h"
#include <pp/diag.h>
#include <cstdarg>

RaritanGenericLogger::RaritanGenericLogger(const LOG_LEVEL logLevel) :
		ILoggerStream(logLevel) { /* Set the parent classes' logger's level to what has been provided as constructor's argument */
}

RaritanGenericLogger::~RaritanGenericLogger() {
}

/**
 * This method is a friend of RaritanGenericLogger class
 * swap() is needed within operator=() to implement to copy and swap paradigm
 */
void swap(RaritanGenericLogger& first, RaritanGenericLogger& second) /* nothrow */ {
	using std::swap;	// Enable ADL

	swap(first.logLevel, second.logLevel);
	swap(first.enabled, second.enabled);
	/* Once we have swapped the members of the two instances... the two instances have actually been swapped */
}

RaritanGenericLogger& RaritanGenericLogger::operator=(RaritanGenericLogger other) {
	::swap(*this, other);
	return *this;
}

void RaritanErrorLogger::log(const char *format, ...) {
	PPD_ERR(format, args);
}

int RaritanErrorLogger::overflow(int c) {
	if (c != EOF) {
		if (c == EOL) {
			PPD_ERR(this->buffer);
			this->buffer = "";
		}
		else {
			this->buffer += c;
		}
	}
	return c;
}

static RaritanErrorLogger raritanErrorLogger;	/* Create a unique instance of the RaritanErrorLogger that will be used to handle error logs */
//static RaritanWarningLogger raritanWarningLogger;	/* Create a unique instance of the RaritanWarningLogger that will be used to handle warning logs */
//static RaritanInfoLogger raritanInfoLogger;	/* Create a unique instance of the RaritanInfoLogger that will be used to handle info logs */
//static RaritanDebugLogger raritanDebugLogger;	/* Create a unique instance of the RaritanDebugLogger that will be used to handle debug logs */
//static RaritanTraceLogger raritanTraceLogger;	/* Create a unique instance of the RaritanTraceLogger that will be used to handle trace logs */

RaritanLogger::RaritanLogger(ILoggerStream& errorLogger, ILoggerStream& warningLogger, ILoggerStream& infoLogger, ILoggerStream& debugLogger, ILoggerStream& traceLogger) :
		ILogger(errorLogger, warningLogger, infoLogger, debugLogger, traceLogger) {
}

RaritanLogger::~RaritanLogger() {
}

RaritanLogger& RaritanLogger::getInstance() {
	/* FIXME: for now we use error logger impl for all logging types */
	static RaritanLogger instance(raritanErrorLogger, raritanErrorLogger, raritanErrorLogger, raritanErrorLogger, raritanErrorLogger); /* Unique instance of the singleton */

	return instance;
}


/* Create unique (global) instances of each logger type, and store them inside the ILogger (singleton)'s class static attribute */
std::ostream ILogger::loggerErrorStream(&RaritanLogger::getInstance().errorLogger);
std::ostream ILogger::loggerWarningStream(&RaritanLogger::getInstance().warningLogger);
std::ostream ILogger::loggerInfoStream(&RaritanLogger::getInstance().infoLogger);
std::ostream ILogger::loggerDebugStream(&RaritanLogger::getInstance().debugLogger);
std::ostream ILogger::loggerTraceStream(&RaritanLogger::getInstance().traceLogger);

/* TODO: convert the old methods below into ILoggerStream methods and remove all RaritanLogger::* methods below */
//void RaritanLogger::outputWarningLog(const char *format, ...) {
//
//	va_list args;
//	va_start(args, format);
//	PPD_WARN(format, args);
//	va_end(args);
//}
//
//void RaritanLogger::outputInfoLog(const char *format, ...) {
//
//	va_list args;
//	va_start(args, format);
//	PPD_INFO(format, args);
//	va_end(args);
//}
//
//void RaritanLogger::outputDebugLog(const char *format, ...) {
//
//	va_list args;
//	va_start(args, format);
//	PPD_DEBUG(format, args);
//	va_end(args);
//}
//
//void RaritanLogger::outputTraceLog(const char *format, ...) {
//
//	va_list args;
//	va_start(args, format);
//	PPD_TRACE(format, args);
//	va_end(args);
//}
