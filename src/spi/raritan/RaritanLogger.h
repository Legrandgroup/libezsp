/**
 * @file RaritanLogger.h
 *
 * @brief Concrete implementation of a logger using Raritan's framework
 */

#pragma once

#include "../ILogger.h"
#include <pp/diag.h>

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

/**
 * @brief Class to interact with a UART in the Raritan framework
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
	void log(const ILogger::LOG_LEVEL log_level, const char *format, ...);

};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
