/**
 * \file IUartDriver.h
 *
 * \brief Abstract interface to which must conforms concrete implementations of classes that manipulate UARTs
 *
 * Used as a dependency inversion paradigm
 */

#pragma once

#include <string>

/**
 * \class IUartDriver
 *
 * \brief Abstract class that manipulate UARTs
 */
class IUartDriver {

public:
	/**
	 * @brief Constructor
	 */
	IUartDriver() {	}

	/**
	 * @brief Destructor
	 */
	virtual ~IUartDriver() { }

	/**
	 * @brief Opens the serial port
	 *
	 * @param serialPortName The name of the serial port to open (eg: "/dev/ttyUSB0")
	 * @param baudRate The baudrate to enforce on the serial port
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
	virtual void open(const std::string& serialPortName, unsigned int baudRate) = 0;

	/**
	 * @brief Writes a byte sequence to the serial port
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
//	virtual void write() = 0;

	/**
	 * @brief Callback to execute on bytes received
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
//	virtual void setReadCallback() = 0;

	/**
	 * @brief Callback to close the serial port
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
	virtual void close() = 0;

public:

	std::vector<unsigned char> sha1Sum;	/*!< The SHA-1 sum as a vector of bytes */
	std::vector<unsigned char> sha256Sum;	/*!< The SHA-256 sum as a vector of bytes */
};
