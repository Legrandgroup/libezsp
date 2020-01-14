/**
 * @file RaritanLogger.h
 *
 * @brief Concrete implementation of a logger using Raritan's framework
 */

#pragma once

/**
 * @brief Macro to allow logger getter to fetch the singleton instance of this logger class
**/
#define SINGLETON_LOGGER_CLASS_NAME RaritanLogger
#include "spi/ILogger.h"
#include <string>

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN
namespace NSSPI {

/**
 * @brief Class to implement error message logging
 */
class RaritanGenericLogger : public ILoggerStream {
public:
	/**
	 * @brief Constructor
	 *
	 * @param setLogLevel The log level handled by this logger instance. This is fixed at construction and cannot be changed afterwards
	 */
	RaritanGenericLogger(const LOG_LEVEL setLogLevel);

	/**
	 * @brief Copy constructor
	 *
	 * @param other The object instance to construct from
	 */
	RaritanGenericLogger(const RaritanGenericLogger& other);

	/**
	 * @brief Destructor
	 */
	virtual ~RaritanGenericLogger();

protected:
	/**
	 * @brief Receive one character of an output stream
	 *
	 * @param c The new character
	 *
	 * @return The character that has actually been printed out to the log
	 */
	virtual int overflow(int c);

protected:
	/* Member variables (attributes) */
	std::string m_buffer;	/*!< The currently built buffer (used when outputting to log using ostream's operator<< */
};

/**
 * @brief Class to implement error message logging, specializing the generic class RaritanGenericLogger
 */
class RaritanErrorLogger : public RaritanGenericLogger {
public:
	/**
	 * @brief Default constructor
	 */
	RaritanErrorLogger() :
		RaritanGenericLogger(LOG_LEVEL::ERROR) {
	}

	/**
	 * @brief Handle a log message
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 *
	 * @param format The format to use
	 */
	virtual void logf(const char *format, ...);

};

/**
 * @brief Class to implement warning message logging, specializing the generic class RaritanGenericLogger
 */
class RaritanWarningLogger : public RaritanGenericLogger {
public:
	/**
	 * @brief Default constructor
	 */
	RaritanWarningLogger() :
		RaritanGenericLogger(LOG_LEVEL::WARNING) {
	}

	/**
	 * @brief Handle a log message
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 *
	 * @param format The format to use
	 */
	virtual void logf(const char *format, ...);
};

/**
 * @brief Class to implement info message logging, specializing the generic class RaritanGenericLogger
 */
class RaritanInfoLogger : public RaritanGenericLogger {
public:
	/**
	 * @brief Default constructor
	 */
	RaritanInfoLogger() :
		RaritanGenericLogger(LOG_LEVEL::INFO) {
	}

	/**
	 * @brief Handle a log message
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 *
	 * @param format The format to use
	 */
	virtual void logf(const char *format, ...);
};

/**
 * @brief Class to implement debug message logging, specializing the generic class RaritanGenericLogger
 */
class RaritanDebugLogger : public RaritanGenericLogger {
public:
	/**
	 * @brief Default constructor
	 */
	RaritanDebugLogger() :
		RaritanGenericLogger(LOG_LEVEL::DEBUG) {
	}

	/**
	 * @brief Handle a log message
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 *
	 * @param format The format to use
	 */
	virtual void logf(const char *format, ...);

};

/**
 * @brief Class to interact with the logger in the Raritan framework
 */
class RaritanLogger : public ILogger {
	friend Logger;
protected:
	/**
	 * @brief Constructor
	 *
	 * @param errorLogger The logger to use for error messages
	 * @param warningLogger The logger to use for warning messages
	 * @param infoLogger The logger to use for info messages
	 * @param debugLogger The logger to use for debug messages
	 * @param traceLogger The logger to use for trace messages
	 */
	RaritanLogger(ILoggerStream& errorLogger, ILoggerStream& warningLogger, ILoggerStream& infoLogger, ILoggerStream& debugLogger, ILoggerStream& traceLogger);

	RaritanLogger();

	virtual ~RaritanLogger() = default;

public:
	/**
	 * @brief Assignment operator
	 *
	 * Copy construction is forbidden on this class, as it is a singleton
	 */
	RaritanLogger& operator=(const RaritanLogger& other) = delete;
};

} // namespace NSSPI

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
