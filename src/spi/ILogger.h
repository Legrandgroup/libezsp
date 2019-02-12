/**
 * @file ILogger.h
 *
 * @brief Abstract interface to which must conforms concrete implementations of classes that output log messages
 *
 * Used as a dependency inversion paradigm
 */

#pragma once

#include <string>
#include <cstdarg>

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
 * and finally, so as to ease to use of the logger in the code, the following defines should also be implemented in the SPI header:
 * @code
 * #define plog MySpecializedLogger::getInstance().outputGenericLog
 * #define plogE MySpecializedLogger::getInstance().outputErrorLog
 * #define plogW MySpecializedLogger::getInstance().outputWarningLog
 * #define plogI MySpecializedLogger::getInstance().outputInfoLog
 * #define plogD MySpecializedLogger::getInstance().outputDebugLog
 * #define plogT MySpecializedLogger::getInstance().outputTraceLog
 * @endcode
 */
class ILogger {
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

};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
