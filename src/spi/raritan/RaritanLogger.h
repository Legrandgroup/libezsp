/**
 * @file RaritanLogger.h
 *
 * @brief Concrete implementation of a logger using Raritan's framework
 */

#pragma once

#include "../ILogger.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

#define SINGLETON_LOGGER_CLASS_NAME RaritanLogger
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
 * @brief Class to interact with the logger in the Raritan framework
 */
class RaritanLogger : public ILogger {
public:
	/**
	 * @brief Default constructor
	 */
	RaritanLogger();

	/**
	 * @brief Copy constructor
	 *
	 * Copy construction is forbidden on this class
	 */
	RaritanLogger(const RaritanLogger& other);

	/**
	 * @brief Assignment operator
	 *
	 * Copy construction is forbidden on this class
	 */
	RaritanLogger& operator=(const RaritanLogger& other) = delete;

	/**
	 * @brief Destructor
	 */
	~RaritanLogger();

	/**
	 * @brief Log a message
	 *
	 * @param log_level The level of the log to generate
	 * @param format The printf-style format followed by a variable list of arguments
	 */
	void outputGenericLog(const ILogger::LOG_LEVEL log_level, const char *format, ...);

};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
