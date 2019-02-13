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
class ConsoleErrorLogger : public ILoggerError {
public:
	ConsoleErrorLogger();

	virtual ~ConsoleErrorLogger();

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
class ConsoleDebugLogger : public ILoggerDebug {
public:
	ConsoleDebugLogger();

	virtual ~ConsoleDebugLogger();

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
	ConsoleLogger(ILoggerError& errorLogger, ILoggerDebug& debugLogger);

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
