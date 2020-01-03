/**
 * @file ILogger.h
 *
 * @brief Abstract interface to which must conforms concrete implementations of classes that output log messages
 *
 * Used as a dependency inversion paradigm
 */

/**
 * @defgroup printf_compat_logger_macros printf-style logging functions
 *
 * The define below is to allow for seamless integration of the SPI, calls to logger will be as easy as invoking plog()
 *
 * plog() takes at least two arguments: the first one is the log level, the second is the format (following printf convention), followed by as many argument as required by the format string
 * plogE() is a shortcut for the error logging, thus
 * @code
 * plogE("Hello %!", "World");
 * @endcode
 * is equivalent to
 * @code
 * plog(LOG_LEVEL::ERROR, "Hello %!", "World");
 * @endcode
 *
 *  @{
 */

/**
 * @brief Generic logger instance getter
 */
#define getLogger SINGLETON_LOGGER_CLASS_NAME::getInstance

/**
 * @brief Generic logger getter (uses debug level)
 */
#define plog getLogger().debugLogger.log
/**
 * @brief Error logger getter
 */
#define plogE getLogger().errorLogger.log
/**
 * @brief Warning logger getter
 */
#define plogW getLogger().warningLogger.log
/**
 * @brief Info logger getter
 */
#define plogI getLogger().infoLogger.log
/**
 * @brief Debug logger getter
 */
#define plogD getLogger().debugLogger.log
/** @} */

/* Note: we are not using pragma once here because we want the defines above to be applied even if include is done multiple times
 * The code below, however, will be include once, so it is "manually" protected from multiple includes using an #ifdef directive
 */
#ifndef __ILOGGER_H__
#define __ILOGGER_H__

#include <string>
#include <cstdarg>
#include <streambuf>
#include <ostream>
#include <iostream>

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

/**
 * @brief Log level description
 */
typedef enum {
	ERROR = 0,
	WARNING,
	INFO,
	DEBUG,
	TRACE
} LOG_LEVEL;

/* FIXME: current implementation uses std::streambuf::overflow(), causing invokation of writes for each character, and thus forcing to re-implement line-split for Raritan's logger
 * we could use a better implementation like:
 * https://stackoverflow.com/questions/2638654/redirect-c-stdclog-to-syslog-on-unix
 */
/**
 * @brief Abstract class to implement and ostream-compatible message logger
 *
 * Specialized loggers should derive from this virtual class in order to provide a concrete implementation of a logging mechanism.
 *
 * A concrete implementation that specializes the ILogger class should also derive logger implementations from ILoggerStream, then instanciate each of these loggers statically in their concrete implementation .cpp file:
 * @code
 * static MyErrorLogger myErrorLoggerInstance;
 * @endcode
 *
 * Once this is done, the myErrorLoggerInstance instance will be provided as the error logger as argument of the ILogger constructor:
 * @code
 * class MySpecializedLogger : public ILogger {
 *
 *     ....
 *
 *     static MySpecializedLogger& getInstance() {
 *         static MySpecializedLogger instance(myErrorLoggerInstance, ...);
 *         return instance;
 *     }
 * };
 * @endcode
 *
 */
class ILoggerStream : public std::streambuf {
public:
	/**
	 * @brief Default constructor
	 *
	 * Construction without arguments is not allowed. See the specific constructor for how to instanciate a ILoggerStream
	 */
	ILoggerStream() = delete;

	/**
	 * @brief Constructor
	 *
	 * @param setLogLevel The log level handled by this logger instance. This is fixed at construction and cannot be changed afterwards
	 * @param isEnabled Is this logger enabled (this can be reset later on using method setEnable()
	 */
	ILoggerStream(const LOG_LEVEL setLogLevel, const bool isEnabled = true) :
		logLevel(setLogLevel),
		enabled(isEnabled),
		muted(false) {
	}

	/**
	 * @brief Destructor
	 */
	virtual ~ILoggerStream() = default;

	/**
	 * @brief Mute this logger output (whatever max log level has been setup)
	 */
	virtual void mute() {
		this->muted = true;
	}

	/**
	 * @brief Unmute this logger output (logs may still not be output depending on the max log level has been setup)
	 */
	virtual void unmute() {
		this->muted = false;
	}

	/**
	 * @brief Is this logger currently allowed to output?
	 *
	 * @return true if this logger outputs, or false if it is either disabled or muted
	 */
	virtual bool isOutputting() const {
		return (this->enabled && !this->muted);
	}

	/**
	 * @brief Set the maximum log level that is enabling loggers
	 *
	 * @param maxLevel The minimum level that is enabled. If the logLevel value of this logger corresponds to this level or lower, then this logger will be enabled. Otherwise, it will be disabled.
	 */
	virtual void setMaxEnabledLogLevel(const LOG_LEVEL maxLevel) {
		this->enabled = (this->logLevel <= maxLevel);
	}

	/**
	 * @brief Handle a log message
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 *
	 * @param format The format to use
	 */
	virtual void log(const char *format, ...) = 0;

protected:
	/**
	 * @brief Receive one character of an output stream
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 *
	 * @note This is the method allowing to implement an ostream out of this class
	 *
	 * @param c The new character
	 *
	 * @return The character that has actually been printed out to the log
	 */
	virtual int overflow(int c) = 0;

protected:
	LOG_LEVEL logLevel;	/*!< The log level handled by this instance of the logger, set at construction, then must not be modified anymore */
	bool enabled;	/*!< Is this logger currently enabled. */
	bool muted;	/*!< Is this logger muted */
};

/**
 * @brief Class to output log messages
 *
 * @note This class implements the singleton design pattern (in the lazy way).
 *
 * Specialized loggers should derive from this virtual class in order to provide a concrete implementation of a logging mechanism.
 *
 * @warning Only one derived logger class header should be included in the client code, if not, only the last includes will dictate which logger is used, or the specific logger's getInstance() will have to be invoked.
 *
 * @warning In addition to deriving from this class, subclasses will need to implement a singleton pattern by declaring a getInstance() method like the following:
 * @code
 * class MySpecializedLogger : public ILogger {
 *
 *     ....
 *
 *     static MySpecializedLogger& getInstance() {
 *         static MySpecializedLogger instance(myErrorLoggerInstance, myWarningLoggerInstance, ...);
 *         return instance;
 *     }
 * };
 * @endcode
 *
 * Also, streams will have to be instanciated for each logger so that an ostream can be used to output to this logger. For example, for our error logger instance, we will also add in the concrete implementation .cpp file:
 * @code
 * std::ostream ILogger::loggerErrorStream(&MySpecializedLogger::getInstance().errorLogger);
 * @endcode
 *
 * and finally, so as to ease to use of the logger in the code, commodity defines are provided (allowing to invoke plog() calles rather than specific getInstance() ones.
 * In order to make this work, SINGLETON_LOGGER_CLASS_NAME should be defined in the SPI header BEFORE this header is included. Here is an example for a logger class MySpecializedLogger that derives from ILogger:
 * @code
 * #define SINGLETON_LOGGER_CLASS_NAME MySpecializedLogger
 * #include "ILogger.h"
 * @endcode
 */
class ILogger {
protected:
	/**
	 * @brief Constructor
	 *
	 * @param usedErrorLogger The logger stream to use for error messages
	 * @param usedWarningLogger The logger stream to use for warning messages
	 * @param usedInfoLogger The logger stream to use for info messages
	 * @param usedDebugLogger The logger stream to use for debug messages
	 * @param usedTraceLogger The logger stream to use for trace messages
	 */
	ILogger(ILoggerStream& usedErrorLogger, ILoggerStream& usedWarningLogger, ILoggerStream& usedInfoLogger, ILoggerStream& usedDebugLogger, ILoggerStream& usedTraceLogger) :
		errorLogger(usedErrorLogger),
		warningLogger(usedWarningLogger),
		infoLogger(usedInfoLogger),
		debugLogger(usedDebugLogger),
		traceLogger(usedTraceLogger)
	{
	}

	/**
	 * @brief Destructor
	 */
	virtual ~ILogger() = default;

	/**
	 * @brief Copy constructor
	 *
	 * Copy construction is forbidden on this class, as it is a singleton
	 */
	ILogger(const ILogger& other) = delete;

public:
	/**
	 * @brief Set logging level
	 *
	 * @param logLevel The minimum level that is enabled. If the logLevel value of this logger corresponds to this level or higher, then this logger will be enabled otherwise, it will be disabled.
	 */
	virtual void setLogLevel(const LOG_LEVEL logLevel) {
		/* Dispatch the requested loglevel to all enclosed loggers */
		this->errorLogger.setMaxEnabledLogLevel(logLevel);
		this->warningLogger.setMaxEnabledLogLevel(logLevel);
		this->infoLogger.setMaxEnabledLogLevel(logLevel);
		this->debugLogger.setMaxEnabledLogLevel(logLevel);
		this->traceLogger.setMaxEnabledLogLevel(logLevel);
	}

	/**
	 * @brief Generic message logger method
	 *
	 * This method will dispatch to the appropriate logging method based on the logLevel argument
	 *
	 * @param logLevel The level of the log to generate
	 * @param format The printf-style format followed by a variable list of arguments
	 */
	virtual void outputGenericLog(const LOG_LEVEL logLevel, const char *format, ...) {
		va_list args;

		va_start(args, format);
		switch (logLevel) {
		case LOG_LEVEL::ERROR:
			this->errorLogger.log(format, args);
			break;
		case LOG_LEVEL::WARNING:
			this->warningLogger.log(format, args);
			break;
		case LOG_LEVEL::INFO:
			this->infoLogger.log(format, args);
			break;
		case LOG_LEVEL::DEBUG:
			this->debugLogger.log(format, args);
			break;
		case LOG_LEVEL::TRACE:
			this->traceLogger.log(format, args);
			break;
		}
		va_end(args);
	}

	ILoggerStream& errorLogger;	/*!< The enclosed error debugger handler instance */
	ILoggerStream& warningLogger;	/*!< The enclosed warning debugger handler instance */
	ILoggerStream& infoLogger;	/*!< The enclosed info debugger handler instance */
	ILoggerStream& debugLogger;	/*!< The enclosed debug debugger handler instance */
	ILoggerStream& traceLogger;	/*!< The enclosed trace debugger handler instance */
	static std::ostream loggerErrorStream;	/*!< A global error ostream */
	static std::ostream loggerWarningStream;	/*!< A global warning ostream */
	static std::ostream loggerInfoStream;	/*!< A global info ostream */
	static std::ostream loggerDebugStream;	/*!< A global debug ostream */
	static std::ostream loggerTraceStream;	/*!< A global trace ostream */
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN

/**
 * @defgroup ostream_compat_logger_macros ostream-style logging functions
 *
 * The defines below allow to log directly through an ostream
 *
 * clog is a default logger stream
 * clogE is the error logger stream
 * thus
 * @code
 * clogE << "Error!";
 * @endcode
 * is equivalent to
 * @code
 * plogE("Error");
 * @endcode
 *
 *  @{
 */

/**
 * @brief Generic logger getter (uses debug level)
 */
#define clog ILogger::loggerDebugStream
/**
 * @brief Error logger getter
 */
#define clogE ILogger::loggerErrorStream
/**
 * @brief Warning logger getter
 */
#define clogW ILogger::loggerWarningStream
/**
 * @brief Info logger getter
 */
#define clogI ILogger::loggerInfoStream
/**
 * @brief Debug logger getter
 */
#define clogD ILogger::loggerDebugStream
/** @} */

#endif // __ILOGGER_H__
