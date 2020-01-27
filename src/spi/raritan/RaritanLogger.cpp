/**
 * @file RaritanLogger.cpp
 *
 * @brief Concrete implementation of a logger using Raritan's framework
 */

#include <cstdarg>

#include "RaritanLogger.h"

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

void RaritanErrorLogger::logf(const char *format, ...) {
	if (this->enabled && !this->muted) {
		va_list args;
		va_start(args, format);
		PPD_ERR(format, args);
		va_end(args);
	}
}

void RaritanWarningLogger::logf(const char *format, ...) {
	if (this->enabled && !this->muted) {
		va_list args;
		va_start(args, format);
		PPD_WARN(format, args);
		va_end(args);
	}
}

void RaritanInfoLogger::logf(const char *format, ...) {
	if (this->enabled && !this->muted) {
		va_list args;
		va_start(args, format);
		PPD_INFO(format, args);
		va_end(args);
	}
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
std::ostream ILogger::loggerErrorStream(&Logger::getInstance()->errorLogger);
std::ostream ILogger::loggerWarningStream(&Logger::getInstance()->warningLogger);
std::ostream ILogger::loggerInfoStream(&Logger::getInstance()->infoLogger);
std::ostream ILogger::loggerDebugStream(&Logger::getInstance()->debugLogger);
std::ostream ILogger::loggerTraceStream(&Logger::getInstance()->traceLogger);

/* TODO: convert the old TRACE methods below into ILoggerStream methods and remove all RaritanLogger::* methods below... today TRACE level messages are handled as DEBUG level messages */
//void RaritanLogger::outputTraceLog(const char *format, ...) {
//
//	va_list args;
//	va_start(args, format);
//	PPD_TRACE(format, args);
//	va_end(args);
//}
