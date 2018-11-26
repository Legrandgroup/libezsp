/**
 * @file SerialUartDriver.h
 *
 * @brief Concrete implementation of a UART driver using libserialcpp
 */

#pragma once


#include "../IUartDriver.h"

#include <thread>
#include "serial/serial.h"

/**
 * @brief Class to interact with a UART using libserialcpp
 */
class UartDriverSerial : public IUartDriver {
public:
	/**
	 * @brief Default constructor
	 */
	UartDriverSerial();

	/**
	 * @brief Destructor
	 */
	~UartDriverSerial();

	/**
	 * @brief Copy constructor
	 *
	 * Copy construction is forbidden on this class
	 */
	UartDriverSerial(const UartDriverSerial& other) = delete;

	/**
	 * @brief Assignment operator
	 *
	 * Copy construction is forbidden on this class
	 */
	UartDriverSerial& operator=(const UartDriverSerial& other) = delete;

	void setIncomingDataHandler(GenericAsyncDataInputObservable* uartIncomingDataHandler);
	void open(const std::string& serialPortName, unsigned int baudRate = 57600);
	int write(size_t& writtenCnt, const void* buf, size_t cnt);
	void close();

private:
	serial::Serial m_serial_port;
	GenericAsyncDataInputObservable *m_data_input_observable;
	volatile bool m_read_thread_alive ;
	std::thread m_read_messages_thread;
};
