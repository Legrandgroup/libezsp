/**
 * @file RaritanUartDriver.h
 */

#pragma once

#include <pp/Selector.h>
#include <pp/File.h>
#include <pp/Tty.h>
#include "../IUartDriver.h"
#include "RaritanEventLoop.h"

class UartDriverRaritan : public IUartDriver {
public:
	UartDriverRaritan(RaritanEventLoop& eventLoop);
	virtual ~UartDriverRaritan();

	virtual void open(const std::string& serialPortName, unsigned int baudRate = 57600);
	virtual int write(size_t& writtenCnt, const void* buf, size_t cnt);
	virtual void close();

private:
	RaritanEventLoop& m_eventLoop;
	pp::Selector::SelectableHandle m_sel_handle;
	pp::Tty::SPtr m_serial_tty;
};
