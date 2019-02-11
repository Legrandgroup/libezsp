/**
 * @file SerialUartDriver.cpp
 *
 * @brief Concrete implementation of a UART driver using libserialcpp
 */

#include "SerialUartDriver.h"

#include <exception>
#include <iostream>	// FIXME: for std::cerr during debug

SerialUartDriver::SerialUartDriver() :
	m_serial_port(),
	m_data_input_observable(nullptr),
	m_read_thread_alive(false),
	m_read_messages_thread() { }

SerialUartDriver::~SerialUartDriver() {
	if (this->m_read_thread_alive) {
		this->m_read_thread_alive = false;
		if (this->m_read_messages_thread.joinable()) {
			this->m_read_messages_thread.join();
		}
	}
	this->close();
}

void SerialUartDriver::setIncomingDataHandler(GenericAsyncDataInputObservable* uartIncomingDataHandler) {
	this->m_data_input_observable = uartIncomingDataHandler;
}

void SerialUartDriver::open(const std::string& serialPortName, unsigned int baudRate) {
	this->m_serial_port.setBaudrate(baudRate);
	this->m_serial_port.setParity(serial::parity_none);
	this->m_serial_port.setStopbits(serial::stopbits_one);
	this->m_serial_port.setPort(serialPortName);

	/*serial::Timeout timeout;
	timeout.read_timeout_constant = 1;
	timeout.write_timeout_constant = -1;*/
	this->m_serial_port.setTimeout(serial::Timeout::max(), -1, 0, -1, 0);
	//this->m_serial_port.flush();

	this->m_serial_port.open();

	if (this->m_serial_port.isOpen()) {
		this->m_read_thread_alive = true;
		this->m_read_messages_thread = std::thread([this]() {
			//std::string readData = "";
			unsigned char readData[1];
			size_t rdcnt;

			while (this->m_read_thread_alive) {
				try {
					rdcnt = this->m_serial_port.read(readData, sizeof(readData)/sizeof(unsigned char));
					if (this->m_data_input_observable)
						this->m_data_input_observable->notifyObservers(readData, rdcnt);
				}
				catch (std::exception& e) {
					std::cerr << "Exception in read: " << e.what() << std::endl;
				}
			}
		});
	}
	else {
		std::cerr << "Serial Port not opened" << std::endl;
	}
}

int SerialUartDriver::write(size_t& writtenCnt, const void* buf, size_t cnt) {
	try {
		writtenCnt =  this->m_serial_port.write(static_cast<const uint8_t*>(buf), cnt);
	}
	catch (std::exception& e) {
		std::cerr << "Exception in write: " << e.what() << std::endl;
		return -1;
	}
	return 0;
}

void SerialUartDriver::close() {
	if (this->m_serial_port.isOpen()) {
		this->m_serial_port.flush();
		this->m_serial_port.close();
	}
}
