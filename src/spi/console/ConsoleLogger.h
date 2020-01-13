/**
 * @file ConsoleLogger.h
 *
 * @brief Concrete implementation of a logger using the stdout and stderr output streams on the text console
 */

#pragma once

/**
 * @brief Macro to allow logger getter to fetch the singleton instance of this logger class
**/
#define SINGLETON_LOGGER_CLASS_NAME ConsoleLogger
#include "spi/ILogger.h"
#include "spi/Logger.h"


namespace NSSPI {

class ConsoleStderrLogger;
void swap(ConsoleStderrLogger& first, ConsoleStderrLogger& second) noexcept;

/**
 * @brief Class to implement error message logging
 */
class ConsoleStderrLogger : public ILoggerStream {
public:
	/**
	 * @brief Constructor
	 *
	 * @param logLevel The log level handled by this logger instance. This is fixed at construction and cannot be changed afterwards
	 */
	explicit ConsoleStderrLogger(const LOG_LEVEL logLevel);

	/**
	 * @brief Output a log message
	 *
	 * @param format The format to use
	 */
	virtual void logf(const char *format, ...);

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
 * @brief Class to implement debug message logging
 */
class ConsoleStdoutLogger : public ILoggerStream {
public:
	/**
	 * @brief Constructor
	 *
	 * @param logLevel The log level handled by this logger instance. This is fixed at construction and cannot be changed afterwards
	 */
	explicit ConsoleStdoutLogger(const LOG_LEVEL logLevel);

	/**
	 * @brief Output a log message
	 *
	 * @param format The format to use
	 */
	virtual void logf(const char *format, ...);

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
 * @brief Class to implement error message logging
 */
class ConsoleErrorLogger : public ConsoleStderrLogger {
public:
	/**
	 * @brief Default constructor, creating an error logger, based on ConsoleStderrLogger
	 */
	ConsoleErrorLogger() :
		ConsoleStderrLogger(LOG_LEVEL::ERROR) {
	}
};

/**
 * @brief Class to implement warning message logging
 */
class ConsoleWarningLogger : public ConsoleStderrLogger {
public:
	/**
	 * @brief Default constructor, creating a warning logger, based on ConsoleStderrLogger
	 */
	ConsoleWarningLogger() :
		ConsoleStderrLogger(LOG_LEVEL::WARNING) {
	}
};

/**
 * @brief Class to implement info message logging
 */
class ConsoleInfoLogger : public ConsoleStdoutLogger {
public:
	/**
	 * @brief Default constructor, creating an info logger, based on ConsoleStdoutLogger
	 */
	ConsoleInfoLogger() :
		ConsoleStdoutLogger(LOG_LEVEL::INFO) {
	}
};

/**
 * @brief Class to implement debug message logging
 */
class ConsoleDebugLogger : public ConsoleStdoutLogger {
public:
	/**
	 * @brief Default constructor, creating a debug logger, based on ConsoleStdoutLogger
	 */
	ConsoleDebugLogger() :
		ConsoleStdoutLogger(LOG_LEVEL::DEBUG) {
	}
};

/**
 * @brief Class to implement trace message logging
 */
class ConsoleTraceLogger : public ConsoleStdoutLogger {
public:
	/**
	 * @brief Default constructor, creating a trace logger, based on ConsoleStdoutLogger
	 */
	ConsoleTraceLogger() :
		ConsoleStdoutLogger(LOG_LEVEL::TRACE) {
	}
};

/**
 * @brief Class to interact with a console logger
 */
class ConsoleLogger : public ILogger {
	friend class Logger;
	friend ILogger* Logger::getInstance();
//protected:
public:
	/**
	 * @brief Default constructor
	 *
	 * @param errorLogger The logger to use for error messages
	 * @param warningLogger The logger to use for warning messages
	 * @param infoLogger The logger to use for info messages
	 * @param debugLogger The logger to use for debug messages
	 * @param traceLogger The logger to use for trace messages
	 */
	ConsoleLogger(ILoggerStream& newErrorLogger, ILoggerStream& newWarningLogger, ILoggerStream& mewInfoLogger, ILoggerStream& newDebugLogger, ILoggerStream& newTraceLogger);

	ConsoleLogger();

	~ConsoleLogger() = default;

public:
	/**
	 * @brief Assignment operator
	 *
	 * Copy construction is forbidden on this class, as it is a singleton
	 */
	ConsoleLogger& operator=(const ConsoleLogger& other) = delete;
};

} // namespace NSSPI
