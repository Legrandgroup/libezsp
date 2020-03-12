/**
 * @file RaritanUartDriver.h
 *
 * @brief Concrete implementation of a UART driver using Raritan's framework
 */

#pragma once

#include <pp/Selector.h>
#include <pp/File.h>
#include <pp/Tty.h>
#include "spi/IUartDriver.h"

namespace NSSPI {

/**
 * @brief Class to interact with a UART in the Raritan framework
 */
class RaritanUartDriver : public IUartDriver {
public:
	/**
	 * @brief Constructor
	 *
	 * @param selector An optional selector instance to use to run timers
	 * @param uartIncomingDataHandler An observable instance that will notify its observer when one or more new bytes have been read, if =nullptr, no notification will be done
	 */
	RaritanUartDriver(pp::Selector& selector = *pp::SelectorSingleton::getInstance(), GenericAsyncDataInputObservable* uartIncomingDataHandler = nullptr);

	/**
	 * @brief Copy constructor
	 *
	 * Copy construction is forbidden on this class
	 */
	RaritanUartDriver(const RaritanUartDriver& other) = delete;

	/**
	 * @brief Assignment operator
	 *
	 * Copy construction is forbidden on this class
	 */
	RaritanUartDriver& operator=(const RaritanUartDriver& other) = delete;

	/**
	 * @brief Destructor
	 */
	~RaritanUartDriver();

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
	 *
	 * @return 0 on success, errno on failure
	 */
	int open(const std::string& serialPortName, unsigned int baudRate);

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
	 */
	int write(size_t& writtenCnt, const uint8_t* buf, size_t cnt);

	/**
	 * @brief Close the serial port
	 */
	void close();

private:
	pp::Selector& m_eventSelector;	/*!< The raritan mainloop */
	pp::Selector::SelectableHandle m_sel_handle;	/*!< A handle on the selectable (to read bytes) */
	pp::Tty::SPtr m_serial_tty;	/*!< The serial port file descriptor */
	GenericAsyncDataInputObservable* m_data_input_observable;	/*!< The observable that will notify observers when new bytes are available on the UART */
};

} // namespace NSSPI
