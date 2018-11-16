/**
 * @file RaritanUartDriver.cpp
 */

#include "RaritanUartDriver.h"
#include <pp/diag.h>

UartDriverRaritan::UartDriverRaritan(RaritanEventLoop& eventLoop, GenericAsyncDataInputObservable* uartIncomingDataHandler) : m_eventLoop(eventLoop), m_sel_handle(), m_serial_tty(), m_data_input_observable(uartIncomingDataHandler) {
}

UartDriverRaritan::~UartDriverRaritan() {
	this->close();
}

void UartDriverRaritan::setIncomingDataHandler(GenericAsyncDataInputObservable* uartIncomingDataHandler) {
	m_data_input_observable = uartIncomingDataHandler;
}

void UartDriverRaritan::open(const std::string& serialPortName, unsigned int baudRate) {
	pp::Tty::UPtr tmpSerialPortUPTR;
	pp::Tty::open(tmpSerialPortUPTR, serialPortName,
		pp::FileDescriptor::AF_READ_WRITE,
		pp::FileDescriptor::CF_OPEN_EXISTING); // Should handle error here (dongle not present)

	m_serial_tty = std::move(tmpSerialPortUPTR);
	m_serial_tty->setParams(static_cast<int>(baudRate), pp::Tty::Parity::None, 8, false, false, true);

	auto cbin = [this](pp::Selector::SelectableHandle&, short events, short&) {
		if (events & pp::Selector::EVENT_POLLIN) {
			unsigned char readData[256];
			size_t rdcnt;
			this->m_serial_tty->read(rdcnt, readData, sizeof(readData)/sizeof(unsigned char));
			PPD_DEBUG_HEX("read from dongle: ", readData, rdcnt);
			if (this->m_data_input_observable)
				this->m_data_input_observable->notifyObservers(readData, rdcnt);
			//this->m_eventLoop.getSelector().stopAsync();
		}
	};
	this->m_eventLoop.getSelector().addSelectable(m_sel_handle, m_serial_tty, POLLIN, cbin);
}

int UartDriverRaritan::write(size_t& writtenCnt, const void* buf, size_t cnt) {
	PPD_DEBUG_HEX("write to dongle: ", buf, cnt);
	int result = this->m_serial_tty->write(writtenCnt, buf, cnt);
	if (result == PP_OK) {
		PPD_DEBUG("Successfully wrote %d bytes", cnt);
		return 0;
	}
	//if (result != PP_OK && result == 0)	/* Never reached, PP_OK is 0, but it is here to enforce the values in base class IUartDriver */
	//	return -1;
	PPD_WARN("Failed writing %d bytes", cnt);
	return result;
}

void UartDriverRaritan::close() {
	m_serial_tty = nullptr; // TODO: Test this (is the port closed when serial port descriptor goes out of scope?)
}
