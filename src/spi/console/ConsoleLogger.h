/**
 * @file ConsoleLogger.h
 *
 * @brief Concrete implementation of a logger using the stdout and stderr output streams on the text console
 */

#pragma once

#define SINGLETON_LOGGER_CLASS_NAME ConsoleLogger
#include "../ILogger.h"

/**
 * @brief Class to implement error message logging
 */
class ConsoleStderrLogger : public ILoggerStream {
public:
	ConsoleStderrLogger(const LOG_LEVEL logLevel);

	virtual void log(const char *format, ...);

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
	ConsoleStdoutLogger(const LOG_LEVEL logLevel);

	virtual void log(const char *format, ...);

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
	ConsoleErrorLogger() :
		ConsoleStderrLogger(LOG_LEVEL::ERROR) {
	}
};

/**
 * @brief Class to implement debug message logging
 */
class ConsoleDebugLogger : public ConsoleStdoutLogger {
public:
	ConsoleDebugLogger() :
		ConsoleStdoutLogger(LOG_LEVEL::DEBUG) {
	}
};

/**
 * @brief Class to interact with a console logger
 */
class ConsoleLogger : public ILogger {
public:
	/**
	 * @brief Default constructor
	 *
	 * @param errorLogger The logger to use for error messages
	 * @param debugLogger The logger to use for debug messages
	 */
	ConsoleLogger(ILoggerStream& errorLogger, ILoggerStream& debugLogger);

	/**
	 * @brief Copy constructor
	 *
	 * Copy construction is forbidden on this class, as it is a singleton
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
	 * Copy construction is forbidden on this class, as it is a singleton
	 */
	ConsoleLogger& operator=(const ConsoleLogger& other) = delete;

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
	 * @brief Log a trace message
	 *
	 * @param format The printf-style format followed by a variable list of arguments
	 */
	void outputTraceLog(const char *format, ...);
};
