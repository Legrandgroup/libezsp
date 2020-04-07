/**
 * @file ash-driver.h
 *
 * @brief ASH serial driver
 **/

#pragma once

#include <cstdint>
#include <memory>	// For std::unique_ptr

#include "spi/GenericAsyncDataInputObservable.h"
#include "spi/TimerBuilder.h"
#include "spi/ByteBuffer.h"
#include "spi/IUartDriver.h"
#include "ezsp/enum-generator.h"
#include "ezsp/ash-codec.h"

namespace NSEZSP {

class AshDriver : public NSSPI::GenericAsyncDataInputObservable, protected NSSPI::ITimerVisitor {
public:
	typedef std::function<int (size_t& writtenCnt, const uint8_t* buf, size_t cnt)> FAshDriverWriteFunc;    /*!< Callback type for method registerSerialWriteFunc() */

	AshDriver() = delete; /* Construction without arguments is not allowed */

	/**
	 * @param ipCb Callback invoked on ASH state change
	 * @param i_timer_builder Timer builder object used to generate timers
	 */
	AshDriver(CAshCallback *ipCb, const NSSPI::TimerBuilder& i_timer_builder);

	AshDriver(const AshDriver&) = delete; /* No copy construction allowed */

	AshDriver& operator=(AshDriver) = delete; /* No assignment allowed */

	/**
	 * @brief Register a serial writer functor
	 *
	 * @param newWriteFunc A callback function of type int func(size_t& writtenCnt, const void* buf, size_t cnt), that we will invoke to write bytes to the serial port we are decoding (or nullptr to disable callbacks)
	 */
	void registerSerialWriter(FAshDriverWriteFunc newWriteFunc);

	/**
	 * @brief Register a serial writer functor that writes to the given uartHandle
	 *
	 * @param uartHandle A UART handle to use to write to the serial port
	 * 
	 * @note This is a shortcut equivalent to registering a callback using registerSerialWriter() with a FAshDriverWriteFunc argument
	 */
	void registerSerialWriter(NSSPI::IUartDriverHandle uartHandle);

	/**
	 * @brief Check if a serial writer functor is registered
	 *
	 * @return true if a serial writer functor is active or false otherwise
	 */
	bool hasARegisteredSerialWriter() const;

	/**
	 * @brief Send an ASH NCP reset frame
	 * 
	 * This frame must be sent to re-initialize the communication with the NCP when an ASH connection is initiated, so that any previous communication state is cancelled
	 * 
	 * @return true If the NCP reset frame was sent successfully (note that when we return true, we don't have any response yet)
	 */
	bool sendResetNCPFrame(void);

	NSSPI::ByteBuffer sendAckFrame(void);

	NSSPI::ByteBuffer sendDataFrame(NSSPI::ByteBuffer i_data);

	NSSPI::ByteBuffer decode(NSSPI::ByteBuffer &i_data);

	bool isConnected() const;

protected:
	void trigger(NSSPI::ITimer* triggeringTimer);

private:
	std::unique_ptr<NSSPI::ITimer> ackTimer;	/*!< A timer checking acknowledgement of the initial RESET (if !stateConnected) of the last ASH DATA frame (if stateConnected) */
	NSEZSP::AshCodec ashCodec;	/*!< ASH codec utility methods */
	FAshDriverWriteFunc serialWriteFunc;   /*!< A function to write bytes to the serial port */
};

} // namespace NSEZSP
