/**
 * @file ConsoleLogger.h
 *
 * @brief Concrete implementation of a logger using the stdout and stderr output streams on the text console
 */

#pragma once

#define SINGLETON_LOGGER_CLASS_NAME ConsoleLogger
#include "../ILogger.h"

/**
 * @brief Class to interact with a console logger
 */
class ConsoleLogger : public ILogger {
public:
	/**
	 * @brief Default constructor
	 */
	ConsoleLogger();

	/**
	 * @brief Copy constructor
	 *
	 * Copy construction is forbidden on this class
	 */
	ConsoleLogger(const ConsoleLogger& other);

	/**
	 * @brief Get a reference to the singleton (only instance) of this logger class
	 *
	 * @return The unique instance as a reference
	 */
	static ConsoleLogger& getInstance();

	/**
	 * @brief Assignment operator
	 *
	 * Copy construction is forbidden on this class
	 */
	ConsoleLogger& operator=(const ConsoleLogger& other) = delete;

	/**
	 * @brief Destructor
	 */
	~ConsoleLogger();

	/**
	 * @brief Log an error message
	 *
	 * @param format The printf-style format followed by a variable list of arguments
	 */
	void outputErrorLog(const char *format, ...);

	/**
	 * @brief Log a warning message
	 *
	 * @param format The printf-style format followed by a variable list of arguments
	 */
	void outputWarningLog(const char *format, ...);

	/**
	 * @brief Log an info message
	 *
	 * @param format The printf-style format followed by a variable list of arguments
	 */
	void outputInfoLog(const char *format, ...);

	/**
	 * @brief Log a debug message
	 *
	 * @param format The printf-style format followed by a variable list of arguments
	 */
	void outputDebugLog(const char *format, ...);

	/**
	 * @brief Log a trace message
	 *
	 * @param format The printf-style format followed by a variable list of arguments
	 */
	void outputTraceLog(const char *format, ...);

	virtual int overflow(int c);

};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
