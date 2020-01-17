/**
 * @file SerialUartDriver.h
 *
 * @brief Concrete implementation of a UART driver using libserialcpp
 */

#pragma once


#include "spi/IUartDriver.h"

#include <thread>
#include "serial/serial.h"
namespace NSSPI {

/**
 * @brief Class to interact with a UART using libserialcpp
 */
class SerialUartDriver : public IUartDriver {
public:
	/**
	 * @brief Default constructor
	 */
	SerialUartDriver();

	/**
	 * @brief Destructor
	 */
	virtual ~SerialUartDriver();

	/**
	 * @brief Copy constructor
	 *
	 * Copy construction is forbidden on this class
	 */
	SerialUartDriver(const SerialUartDriver& other) = delete;

	/**
	 * @brief Assignment operator
	 *
	 * Copy construction is forbidden on this class
	 */
	SerialUartDriver& operator=(const SerialUartDriver& other) = delete;

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
	void close() final;

private:
	serial::Serial m_serial_port;	/*!< The serial port in use for read/writes */
	GenericAsyncDataInputObservable *m_data_input_observable;		/*!< The observable that will notify observers when new bytes are available on the UART */
	volatile bool m_read_thread_alive;	/*!< A boolean, indicating whether the secondary thread m_read_messages_thread is running */
	std::thread m_read_messages_thread;	/*!< The secondary thread that will block on serial read */
};

} // namespace NSSPI
