/**
 * @file RaritanUartDriver.cpp
 *
 * @brief Concrete implementation of a UART driver using Raritan's framework
 */

#include <pp/diag.h>
#include "spi/Logger.h"
#include "spi/GenericAsyncDataInputObservable.h"

#include "RaritanUartDriver.h"

using NSSPI::GenericAsyncDataInputObservable;
using NSSPI::RaritanUartDriver;
using NSSPI::RaritanLogger;

RaritanUartDriver::RaritanUartDriver(GenericAsyncDataInputObservable* uartIncomingDataHandler) :
	m_eventSelector(*pp::SelectorSingleton::getInstance()),
	m_sel_handle(),
	m_serial_tty(),
	m_data_input_observable(uartIncomingDataHandler)
{
}

RaritanUartDriver::~RaritanUartDriver() {
	this->close();
}

void RaritanUartDriver::setIncomingDataHandler(GenericAsyncDataInputObservable* uartIncomingDataHandler) {
	m_data_input_observable = uartIncomingDataHandler;
}

int RaritanUartDriver::open(const std::string& serialPortName, unsigned int baudRate) {
	pp::Tty::UPtr tmpSerialPortUPTR;
	int err;
	if (PP_FAILED(err = pp::Tty::open(tmpSerialPortUPTR, serialPortName, pp::FileDescriptor::AF_READ_WRITE, pp::FileDescriptor::CF_OPEN_EXISTING)))	{
		return err;
	}

	m_serial_tty = std::move(tmpSerialPortUPTR);
	m_serial_tty->setParams(static_cast<int>(baudRate), pp::Tty::Parity::None, 8, false, false, true);

	auto cbin = [this](pp::Selector::SelectableHandle&, short events, short&) {
		if (events & pp::Selector::EVENT_POLLIN) {
			unsigned char readData[256];
			size_t rdcnt;
			int error;
			if (PP_FAILED(error = this->m_serial_tty->read(rdcnt, readData, sizeof(readData)/sizeof(unsigned char)))) {
				clogE << "Tty.read(): " << error << "\n";
				return;
			}

			if (RaritanLogger::getInstance().debugLogger.isOutputting())	/* Before directly using PPD_DEBUG_*, make sure DEBUG level logs are activated */
				PPD_DEBUG_HEX("read from dongle: ", readData, rdcnt);
			if (this->m_data_input_observable)
				this->m_data_input_observable->notifyObservers(readData, rdcnt);
			//this->m_eventSelector.stopAsync();
		}
	};
	this->m_eventSelector.addSelectable(m_sel_handle, m_serial_tty, POLLIN, cbin);
	return PP_OK;
}

int RaritanUartDriver::write(size_t& writtenCnt, const uint8_t* buf, size_t cnt) {
	if (RaritanLogger::getInstance().debugLogger.isOutputting())	/* Before directly using PPD_DEBUG_*, make sure DEBUG level logs are activated */
		PPD_DEBUG_HEX("write to dongle: ", buf, cnt);
	int result = this->m_serial_tty->write(writtenCnt, buf, cnt);
	if (result == PP_OK) {
		//plogD("Successfully wrote %d bytes", cnt);
		return 0;
	}
	clogW << "Failed writing " << cnt << "bytes" << "\n";
	return result;
}

void RaritanUartDriver::close() {
	m_serial_tty = nullptr; // TODO: Test this (is the port closed when serial port descriptor goes out of scope?)
}
