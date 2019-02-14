/**
 * @file ILogger.h
 *
 * @brief Abstract interface to which must conforms concrete implementations of classes that output log messages
 *
 * Used as a dependency inversion paradigm
 */

/**
 * @brief The define below is to allow for seamless integration of the SPI, calls to logger will be as easy as invoking log()
 */
#define plog SINGLETON_LOGGER_CLASS_NAME::getInstance().outputGenericLog
#define plogE SINGLETON_LOGGER_CLASS_NAME::getInstance().outputErrorLog
#define plogW SINGLETON_LOGGER_CLASS_NAME::getInstance().outputWarningLog
#define plogI SINGLETON_LOGGER_CLASS_NAME::getInstance().outputInfoLog
#define plogD SINGLETON_LOGGER_CLASS_NAME::getInstance().outputDebugLog
#define plogT SINGLETON_LOGGER_CLASS_NAME::getInstance().outputTraceLog

/**
 * @brief The defines below allow to log directly through an ostream
 *
 * clog is a default logger stream
 * clogE is the error logger stream
 * thus
 * @code
 * clogE << "Error!";
 * @endcode
 * is equivalent to
 * @code
 * plogE("Error");
 * @endcode
 */
#define clog ILogger::loggerDebugStream
#define clogE ILogger::loggerErrorStream
#define clogW ILogger::loggerWarningStream
#define clogI ILogger::loggerInfoStream
#define clogD ILogger::loggerDebugStream

/* Note: we are not using pragma once here because we want the defines above to be applied even if include is done multiple times
 * The code below, however, will be include once, so it is "manually" protected from multiple includes using an #ifdef directive
 */
#ifndef __ILOGGER_H__
#define __ILOGGER_H__

#include <string>
#include <cstdarg>
#include <streambuf>
#include <ostream>

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

/**
 * @brief Log level description
 */
typedef enum {
	ERROR = 0,
	WARNING,
	INFO,
	DEBUG,
	TRACE
} LOG_LEVEL;

/**
 * @brief Abstract class to implement and ostream-compatilbe message logger
 *
 * Specialized loggers should derive from this virtual class in order to provide a concrete implementation of a logging mechanism.
 */
class ILoggerStream : public std::streambuf {
public:
	ILoggerStream(const LOG_LEVEL logLevel, const bool enabled = true) :
		logLevel(logLevel),
		enabled(enabled) {
	}

	virtual ~ILoggerStream() { }

	/**
	 * @brief Enable/disable this logger output
	 *
	 * @param enabled Should this logger be enabled?
	 */
	virtual void setEnable(const bool enabled = true) {
		this->enabled = enabled;
	}

	/**
	 * @brief Set the minimum log level that is enabling loggers
	 *
	 * @param minLevel The minimum level that is enabled. If the logLevel value of this logger corresponds to this level or higher, then this logger will be enabled otherwise, it will be disabled.
	 */
	virtual void setMinEnabledLogLevel(const LOG_LEVEL minLevel) {
		this->enabled = (this->logLevel > minLevel);
	}

	/**
	 * @brief Output a log message
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 *
	 * @param format The format to use
	 */
	virtual void log(const char *format, ...) = 0;

protected:
	/**
	 * @brief Receive one character of an output stream
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 *
	 * @note This is the method allowing to implement an ostream out of this class
	 *
	 * @param c The new character
	 *
	 * @return The character that has actually been printed out to the log
	 */
	virtual int overflow(int c) = 0;

protected:
	LOG_LEVEL logLevel;	/*!< The log level handled by this instance of the logger */
	bool enabled;	/*!< Is this logger currently enabled. */
};

/**
 * @brief Abstract singleton class to output log messages
 *
 * Specialized loggers should derive from this virtual class in order to provide a concrete implementation of a logging mechanism.
 *
 * @warning Only one derived logger class header should be included in the client code, if not, only the last includes will dictate which logger is used, or the specific logger's getInstance() will have to be invoked.
 *
 * @warning In addition to deriving from this class, subclasses will need to implement a singleton pattern by declaring a getInstance() method like the following:
 * @code
 * static MySpecializedLogger& getInstance() {
 *     static MySpecializedLogger instance;
 *     return instance;
 * }
 * @endcode
 *
 * and finally, so as to ease to use of the logger in the code, commodity defines are provided (allowing to invoke plog() calles rather than specific getInstance() ones.
 * In order to make this work, SINGLETON_LOGGER_CLASS_NAME should be defined in the SPI header BEFORE this header is included. Here is an example for a logger class MySpecializedLogger that derives from ILogger:
 * @code
 * #define SINGLETON_LOGGER_CLASS_NAME MySpecializedLogger
 * #include "ILogger.h"
 * @endcode
 */
class ILogger {
public:
	/**
	 * @brief Constructor
	 */
	ILogger(ILoggerStream& errorLogger, ILoggerStream& warningLogger, ILoggerStream& infoLogger, ILoggerStream& debugLogger, ILoggerStream& traceLogger) :
		errorLogger(errorLogger),
		warningLogger(warningLogger),
		infoLogger(infoLogger),
		debugLogger(debugLogger),
		traceLogger(traceLogger) {
	}

	/**
	 * @brief Destructor
	 */
	virtual ~ILogger() { }

	/**
	 * @brief Set logging level
	 *
	 * @param logLevel The minimum level that is enabled. If the logLevel value of this logger corresponds to this level or higher, then this logger will be enabled otherwise, it will be disabled.
	 */
	virtual void setLogLevel(const LOG_LEVEL logLevel) {
		/* Dispatch the requested loglevel to all enclosed loggers */
		this->errorLogger.setMinEnabledLogLevel(logLevel);
		this->warningLogger.setMinEnabledLogLevel(logLevel);
		this->infoLogger.setMinEnabledLogLevel(logLevel);
		this->debugLogger.setMinEnabledLogLevel(logLevel);
		this->traceLogger.setMinEnabledLogLevel(logLevel);
	}

	/**
	 * @brief Generic message logger method
	 *
	 * This method will dispatch to the appropriate logging method based on the logLevel argument
	 *
	 * @param logLevel The level of the log to generate
	 * @param format The printf-style format followed by a variable list of arguments
	 */
	virtual void outputGenericLog(const LOG_LEVEL logLevel, const char *format, ...) {
		va_list args;

		va_start(args, format);
		switch (logLevel) {
		case LOG_LEVEL::ERROR:
			this->errorLogger.log(format, args);
			break;
		case LOG_LEVEL::WARNING:
			this->warningLogger.log(format, args);
			break;
		case LOG_LEVEL::INFO:
			this->infoLogger.log(format, args);
			break;
		case LOG_LEVEL::DEBUG:
			this->debugLogger.log(format, args);
			break;
		case LOG_LEVEL::TRACE:
			this->traceLogger.log(format, args);
			break;
		}
		va_end(args);
	}

public:
	ILoggerStream& errorLogger;	/*!< The enclosed error debugger handler instance */
	ILoggerStream& warningLogger;	/*!< The enclosed warning debugger handler instance */
	ILoggerStream& infoLogger;	/*!< The enclosed info debugger handler instance */
	ILoggerStream& debugLogger;	/*!< The enclosed debug debugger handler instance */
	ILoggerStream& traceLogger;	/*!< The enclosed trace debugger handler instance */
	static std::ostream loggerErrorStream;	/*!< A global error ostream */
	static std::ostream loggerWarningStream;	/*!< A global warning ostream */
	static std::ostream loggerInfoStream;	/*!< A global info ostream */
	static std::ostream loggerDebugStream;	/*!< A global debug ostream */
	static std::ostream loggerTraceStream;	/*!< A global trace ostream */
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN

#endif // __ILOGGER_H__
