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
	UartDriverRaritan(RaritanEventLoop& eventLoop, GenericAsyncDataInputObservable* uartIncomingDataHandler = nullptr);
	UartDriverRaritan(UartDriverRaritan& other) = delete;
	virtual ~UartDriverRaritan();

	/**
	 * @brief Set the incoming data handler (a derived class of GenericAsyncDataInputObservable) that will notify observers when new bytes are available on the UART
	 *
	 * @param GenericAsyncDataInputObservable A pointer to the new handler (the eventual previous handler that might have been set at construction will be dropped)
	 */
	void setIncomingDataHandler(GenericAsyncDataInputObservable* uartIncomingDataHandler);
	/**
	 * @brief Opens the serial port
	 *
	 * @param serialPortName The name of the serial port to open (eg: "/dev/ttyUSB0")
	 * @param baudRate The baudrate to enforce on the serial port
	 */
	virtual void open(const std::string& serialPortName, unsigned int baudRate = 57600);
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
	virtual int write(size_t& writtenCnt, const void* buf, size_t cnt);
	/**
	 * @brief Close the serial port
	 *
	 * @warning Not implemented
	 */
	virtual void close();

private:
	RaritanEventLoop& m_eventLoop;
	pp::Selector::SelectableHandle m_sel_handle;
	pp::Tty::SPtr m_serial_tty;
	GenericAsyncDataInputObservable* m_data_input_observable;
};
