/**
 * \file Ifw-downloader.h
 *
 * \brief Abstract interface to which must conforms concrete implementations of classes that download firmware images
 *
 * Used as a dependency inversion paradigm
 */

#pragma once

#include <string>

/**
 * \class IUartDriver
 *
 * \brief Abstract class to download firmware images
 */
class IUartDriver {

public:
	/**
	 * \brief Constructor
	 *
	 * \param sourceLocation The location where to download the firmware image from
	 * \param destinationStream The local stream where we should write the firmware image content
	 * \param sourceFileSize The expected size of the firmware image to download (in bytes), or -1 if unknown
	 */
	IUartDriver() {	}

	virtual ~IUartDriver() { }

	/**
	 * @brief Callback to open the serial port
	 *
	 * @param serialPortName The name of the serial port to open (eg: "/dev/ttyUSB0")
	 * @param baudRate The baudrate to enforce on the serial port
	 *
	 * This method is purely virtual and should be overridden by inheriting classes to implement the real file download
	 */
	virtual void open(const std::string& serialPortName, unsigned int baudRate) = 0;

	/**
	 * \brief Callback to write to the serial port
	 *
	 * This method is purely virtual and should be overridden by inheriting classes to implement the real file download
	 */
//	virtual void write() = 0;

	/**
	 * \brief Callback to execute on bytes received
	 *
	 * This method is purely virtual and should be overridden by inheriting classes to implement the real file download
	 */
//	virtual void setReadCallback() = 0;

	/**
	 * \brief Callback to close the serial port
	 *
	 * This method is purely virtual and should be overridden by inheriting classes to implement the real file download
	 */
	virtual void close() = 0;

public:

	std::vector<unsigned char> sha1Sum;	/*!< The SHA-1 sum as a vector of bytes */
	std::vector<unsigned char> sha256Sum;	/*!< The SHA-256 sum as a vector of bytes */
};
