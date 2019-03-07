/**
 * @file RaritanLogger.h
 *
 * @brief Concrete implementation of a logger using Raritan's framework
 */

#pragma once

#define SINGLETON_LOGGER_CLASS_NAME RaritanLogger
#include "../ILogger.h"
#include <string>

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
	 * @brief Constructor
	 *
	 * @param logLevel The log level handled by this logger instance. This is fixed at construction and cannot be changed afterwards
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
	friend void (::swap)(RaritanErrorLogger& first, RaritanErrorLogger& second);

	/**
	 * @brief Assignment operator
	 * @param other The object to assign to the lhs
	 *
	 * @return The object that has been assigned the value of @p other
	 */
	RaritanErrorLogger& operator=(RaritanErrorLogger other);
};

/**
 * @brief Class to implement warning message logging, specializing the generic class RaritanGenericLogger
 */
class RaritanWarningLogger : public RaritanGenericLogger {
public:
	/**
	 * @brief Constructor
	 *
	 * @param logLevel The log level handled by this logger instance. This is fixed at construction and cannot be changed afterwards
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
	virtual void log(const char *format, ...);

	/**
	 * @brief swap function to allow implementing of copy-and-swap idiom on members of type RaritanWarningLogger
	 *
	 * This function will swap all attributes of @p first and @p second
	 * See http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
	 *
	 * @param first The first object
	 * @param second The second object
	 */
	friend void (::swap)(RaritanWarningLogger& first, RaritanWarningLogger& second);

	/**
	 * @brief Assignment operator
	 * @param other The object to assign to the lhs
	 *
	 * @return The object that has been assigned the value of @p other
	 */
	RaritanWarningLogger& operator=(RaritanWarningLogger other);
};

/**
 * @brief Class to implement info message logging, specializing the generic class RaritanGenericLogger
 */
class RaritanInfoLogger : public RaritanGenericLogger {
public:
	/**
	 * @brief Constructor
	 *
	 * @param logLevel The log level handled by this logger instance. This is fixed at construction and cannot be changed afterwards
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
	virtual void log(const char *format, ...);

	/**
	 * @brief swap function to allow implementing of copy-and-swap idiom on members of type RaritanInfoLogger
	 *
	 * This function will swap all attributes of @p first and @p second
	 * See http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
	 *
	 * @param first The first object
	 * @param second The second object
	 */
	friend void (::swap)(RaritanInfoLogger& first, RaritanInfoLogger& second);

	/**
	 * @brief Assignment operator
	 * @param other The object to assign to the lhs
	 *
	 * @return The object that has been assigned the value of @p other
	 */
	RaritanInfoLogger& operator=(RaritanInfoLogger other);
};

/**
 * @brief Class to implement debug message logging, specializing the generic class RaritanGenericLogger
 */
class RaritanDebugLogger : public RaritanGenericLogger {
public:
	/**
	 * @brief Constructor
	 *
	 * @param logLevel The log level handled by this logger instance. This is fixed at construction and cannot be changed afterwards
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
	virtual void log(const char *format, ...);

	/**
	 * @brief swap function to allow implementing of copy-and-swap idiom on members of type RaritanDebugLogger
	 *
	 * This function will swap all attributes of @p first and @p second
	 * See http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
	 *
	 * @param first The first object
	 * @param second The second object
	 */
	friend void (::swap)(RaritanDebugLogger& first, RaritanDebugLogger& second);

	/**
	 * @brief Assignment operator
	 * @param other The object to assign to the lhs
	 *
	 * @return The object that has been assigned the value of @p other
	 */
	RaritanDebugLogger& operator=(RaritanDebugLogger other);
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
	RaritanLogger& operator=(const RaritanLogger& other) = delete;
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
