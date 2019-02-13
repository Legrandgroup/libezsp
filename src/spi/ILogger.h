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
 */
#define clog ILogger::loggerStream

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
class ILogger : public std::streambuf {
public:
	typedef enum {
		ERROR = 0,
		WARNING,
		INFO,
		DEBUG,
		TRACE
	} LOG_LEVEL;

public:
	/**
	 * @brief Constructor
	 */
	ILogger() {	}

	/**
	 * @brief Destructor
	 */
	virtual ~ILogger() { }

	/**
	 * @brief Generic message logger method
	 *
	 * This method will dispatch to the appropriate logging method based on the logLevel argument
	 *
	 * @param log_level The level of the log to generate
	 * @param format The printf-style format followed by a variable list of arguments
	 */
	virtual void outputGenericLog(const ILogger::LOG_LEVEL logLevel, const char *format, ...) {
		va_list args;

		va_start(args, format);
		switch (logLevel) {
		case ILogger::LOG_LEVEL::ERROR:
			this->outputErrorLog(format, args);
			break;
		case ILogger::LOG_LEVEL::WARNING:
			this->outputWarningLog(format, args);
			break;
		case ILogger::LOG_LEVEL::INFO:
			this->outputInfoLog(format, args);
			break;
		case ILogger::LOG_LEVEL::DEBUG:
			this->outputDebugLog(format, args);
			break;
		case ILogger::LOG_LEVEL::TRACE:
			this->outputTraceLog(format, args);
			break;
		}
		va_end(args);
	}

	/**
	 * @brief Log an error message
	 *
	 * @param format The printf-style format followed by a variable list of arguments
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
	virtual void outputErrorLog(const char *format, ...) = 0;

	/**
	 * @brief Log a warning message
	 *
	 * @param format The printf-style format followed by a variable list of arguments
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
	virtual void outputWarningLog(const char *format, ...) = 0;

	/**
	 * @brief Log an info message
	 *
	 * @param format The printf-style format followed by a variable list of arguments
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
	virtual void outputInfoLog(const char *format, ...) = 0;

	/**
	 * @brief Log a debug message
	 *
	 * @param format The printf-style format followed by a variable list of arguments
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
	virtual void outputDebugLog(const char *format, ...) = 0;
	
	/**
	 * @brief Log a trace message
	 *
	 * @param format The printf-style format followed by a variable list of arguments
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
	virtual void outputTraceLog(const char *format, ...) = 0;

	/*
	 * TODO: also allow logging via std::streambufs
	 * See http://gcc.gnu.org/onlinedocs/libstdc++/manual/streambufs.html
	 */

protected:
	/**
	 * @brief Receive one character of an output stream
	 *
	 * @param c The new character
	 *
	 * @return The character that has actually been printed out to the log
	 */
	virtual int overflow(int c) = 0;

public:
	static std::ostream loggerStream;
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN

#endif // __ILOGGER_H__
