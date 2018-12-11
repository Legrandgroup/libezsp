/**
 * @file RaritanUartDriver.h
 *
 * @brief Concrete implementation of a UART driver using Raritan's framework
 */

#pragma once

#include <pp/Selector.h>
#include <pp/File.h>
#include <pp/Tty.h>
#include "../IUartDriver.h"
#include "RaritanEventLoop.h"

/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>

/**
 * @brief Class to interact with a UART in the Raritan framework
 */
class UartDriverRaritan : public IUartDriver {
public:
	/**
	 * @brief Default constructor
	 *
	 * Construction without arguments is not allowed
	 */
	UartDriverRaritan() = delete;

	/**
	 * @brief Constructor
	 *
	 * @param eventLoop A RaritanEventLoop object to access the mainloop selector
	 * @param uartIncomingDataHandler An observable instance that will notify its observer when one or more new bytes have been read, if =nullptr, no notification will be done
	 */
	UartDriverRaritan(RaritanEventLoop& eventLoop, GenericAsyncDataInputObservable* uartIncomingDataHandler = nullptr);

	/**
	 * @brief Copy constructor
	 *
	 * Copy construction is forbidden on this class
	 */
	UartDriverRaritan(const UartDriverRaritan& other) = delete;

	/**
	 * @brief Assignment operator
	 *
	 * Copy construction is forbidden on this class
	 */
	UartDriverRaritan& operator=(const UartDriverRaritan& other) = delete;

	/**
	 * @brief Destructor
	 */
	~UartDriverRaritan();

	/**
	 * @brief Set the incoming data handler (a derived class of GenericAsyncDataInputObservable) that will notify observers when new bytes are available on the UART
	 *
	 * @param uartIncomingDataHandler A pointer to the new handler (the eventual previous handler that might have been set at construction will be dropped)
	 */
	void setIncomingDataHandler(GenericAsyncDataInputObservable* uartIncomingDataHandler);

	/**
	 * @brief Opens the serial port
	 *
	 * @param serialPortName The name of the serial port to open (eg: "/dev/ttyUSB0")
	 * @param baudRate The baudrate to enforce on the serial port
	 */
	void open(const std::string& serialPortName, unsigned int baudRate = 57600);

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
	int write(size_t& writtenCnt, const void* buf, size_t cnt);

	/**
	 * @brief Close the serial port
	 */
	void close();

private:
	RaritanEventLoop& m_eventLoop;	/*!< The raritan mainloop */
	pp::Selector::SelectableHandle m_sel_handle;	/*!< A handle on the selectable (to read bytes) */
	pp::Tty::SPtr m_serial_tty;	/*!< The serial port file descriptor */
	GenericAsyncDataInputObservable* m_data_input_observable;	/*!< The observable that will notify observers when new bytes are available on the UART */
};

#include <pp/official_api_end.h>
