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
 * @brief Class to implement error message logging
 */
class RaritanGenericLogger : public ILoggerStream {
public:
	/**
	 * @brief Constructor
	 *
	 * @param logLevel The log level handled by this logger instance. This is fixed at construction and cannot be changed afterwards
	 */
	RaritanGenericLogger(const LOG_LEVEL logLevel);

	/**
	 * @brief Copy constructor
	 *
	 * @param other The object instance to construct from
	 */
	RaritanGenericLogger(const RaritanErrorLogger& other);

	/**
	 * @brief Destructor
	 */
	virtual ~RaritanGenericLogger();

	/**
	 * @brief Output a log message
	 *
	 * @param format The format to use
	 */
	virtual void log(const char *format, ...);

	/**
	 * @brief swap function to allow implementing of copy-and-swap idiom on members of type RaritanErrorLogger
	 *
	 * This function will swap all attributes of @p first and @p second
	 * See http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
	 *
	 * @param first The first object
	 * @param second The second object
	 */
	friend void (::swap)(RaritanGenericLogger& first, RaritanGenericLogger& second);

	/**
	 * @brief Assignment operator
	 * @param other The object to assign to the lhs
	 *
	 * @return The object that has been assigned the value of @p other
	 */
	RaritanGenericLogger& operator=(RaritanGenericLogger other);

protected:
	/**
	 * @brief Receive one character of an output stream
	 *
	 * @param c The new character
	 *
	 * @return The character that has actually been printed out to the log
	 */
	virtual int overflow(int c);
};

/**
 * @brief Class to implement error message logging, specializing the generic class RaritanGenericLogger
 */
class RaritanErrorLogger : public RaritanGenericLogger {
public:
	/**
	 * @brief Constructor
	 *
	 * @param logLevel The log level handled by this logger instance. This is fixed at construction and cannot be changed afterwards
	 */
	RaritanErrorLogger(const LOG_LEVEL logLevel) :
		RaritanGenericLogger(LOG_LEVEL::ERROR) {
	}

	virtual void log(const char *format, ...);

protected:
	virtual int overflow(int c);

/* Member variables (attributes) */
	std::string buffer;	/*!< The currently built buffer (used when outputting to log using ostream's operator<< */
};


/**
 * @brief Class to interact with the logger in the Raritan framework
 */
class RaritanLogger : public ILogger {
private:
	/**
	 * @brief Default constructor
	 *
	 * @param errorLogger The logger to use for error messages
	 * @param warningLogger The logger to use for warning messages
	 * @param infoLogger The logger to use for info messages
	 * @param debugLogger The logger to use for debug messages
	 * @param traceLogger The logger to use for trace messages
	 */
	RaritanLogger(ILoggerStream& errorLogger, ILoggerStream& warningLogger, ILoggerStream& infoLogger, ILoggerStream& debugLogger, ILoggerStream& traceLogger);

	~RaritanLogger();

public:
	/**
	 * @brief Get a reference to the singleton (only instance) of this logger class
	 *
	 * @return The unique instance as a reference
	 */
	static RaritanLogger& getInstance();

	/**
	 * @brief Assignment operator
	 *
	 * Copy construction is forbidden on this class, as it is a singleton
	 */
	RaritanLogger& operator=(const ConsoleLogger& other) = delete;
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
