/**
 * @file ILogger.h
 *
 * @brief Abstract interface to which must conforms concrete implementations of classes that output log messages
 *
 * Used as a dependency inversion paradigm
 */

#pragma once

#include <string>

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

/**
 * @brief Abstract class that output log messages
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
	 * @brief Log a message
	 *
	 * @param log_level The level of the log to generate
	 * @param format The printf-style format followed by a variable list of arguments
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
	virtual void log(const ILogger::LOG_LEVEL log_level, const char *format, ...) = 0;

	/**
	 * @brief Log a, error message
	 *
	 * @param format The printf-style format followed by a variable list of arguments
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
	virtual void logE(const char *format, ...) = 0;

	/**
	 * @brief Log a warning message
	 *
	 * @param format The printf-style format followed by a variable list of arguments
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
	virtual void logW(const char *format, ...) = 0;

	/**
	 * @brief Log an info message
	 *
	 * @param format The printf-style format followed by a variable list of arguments
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
	virtual void logI(const char *format, ...) = 0;

	/**
	 * @brief Log a debug message
	 *
	 * @param format The printf-style format followed by a variable list of arguments
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
	virtual void logD(const char *format, ...) = 0;

};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
