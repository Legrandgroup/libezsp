/**
 * @file IUartDriver.h
 *
 * @brief Abstract interface to which must conforms concrete implementations of classes that manipulate UARTs
 *
 * Used as a dependency inversion paradigm
 */

#pragma once

#include <string>

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

class GenericAsyncDataInputObservable;

/**
 * @brief Abstract class that manipulate UARTs
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
	virtual ~IUartDriver() = default;

	/**
	 * @brief Set the incoming data handler (a derived class of GenericAsyncDataInputObservable) that will notify observers when new bytes are available on the UART
	 *
	 * @param uartIncomingDataHandler A pointer to the new handler (the eventual previous handler that might have been set at construction will be dropped)
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
	virtual void setIncomingDataHandler(GenericAsyncDataInputObservable* uartIncomingDataHandler) = 0;

	/**
	 * @brief Opens the serial port
	 *
	 * @param serialPortName The name of the serial port to open (eg: "/dev/ttyUSB0")
	 * @param baudRate The baudrate to enforce on the serial port
	 *
	 * @return 0 on success, errno on failure
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
	virtual int open(const std::string& serialPortName, unsigned int baudRate) = 0;

	/**
	 * @brief Write a byte sequence to the serial port
	 *
	 * NOTE: This function may write less data than requested!
	 *
	 * @param[out] writtenCnt How many bytes were actually written
	 * @param[in] buf data buffer to write
	 * @param[in] cnt byte count of data to write
	 *
	 * @return 0 on success, errno on failure
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
	virtual int write(size_t& writtenCnt, const void* buf, size_t cnt) = 0;

	/**
	 * @brief Callback to close the serial port
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
	 */
	virtual void close() = 0;
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
