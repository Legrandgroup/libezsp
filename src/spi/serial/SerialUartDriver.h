/**
 * @filevSerialUartDriver.h
 */

#pragma once


#include "../IUartDriver.h"

#include <thread>
#include "serial/serial.h"

class UartDriverSerial : public IUartDriver {
public:
	UartDriverSerial(GenericAsyncDataInputObservable& uartIncomingDataHandler);
	virtual ~UartDriverSerial();

	virtual void open(const std::string& serialPortName, unsigned int baudRate = 57600);
	virtual int write(size_t& writtenCnt, const void* buf, size_t cnt);
	virtual void close();

private:
	serial::Serial m_serial_port;
	GenericAsyncDataInputObservable& m_data_input_observable;
	volatile bool m_read_thread_alive ;
	std::thread m_read_messages_thread;

	//Function called by the thread m_input_messages_thread
	void *threadRead(void);
};
