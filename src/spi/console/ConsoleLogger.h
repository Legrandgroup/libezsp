/**
 * @file ConsoleLogger.h
 *
 * @brief Concrete implementation of a logger using the stdout and stderr output streams on the text console
 */

#pragma once

#include "../ILogger.h"

#define SINGLETON_LOGGER_CLASS_NAME ConsoleLogger
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
	 * Get a reference to the only instance of ConsoleLogger
	 */
	static ConsoleLogger& getInstance() {
		static ConsoleLogger instance; /* Unique instance of the singleton */

		return instance;
	}

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

};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
