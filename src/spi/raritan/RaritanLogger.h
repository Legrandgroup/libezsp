/**
 * @file RaritanLogger.h
 *
 * @brief Concrete implementation of a logger using Raritan's framework
 */

#pragma once

#define SINGLETON_LOGGER_CLASS_NAME RaritanLogger
#include "../ILogger.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

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
