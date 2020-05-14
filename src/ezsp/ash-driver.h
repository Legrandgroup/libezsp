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

/* This class is an observer of the serial port
it is also observable by whoever wants to receive decoded ASH frame payloads */
class AshDriver : public NSSPI::GenericAsyncDataInputObservable, public NSSPI::IAsyncDataInputObserver, protected NSSPI::ITimerVisitor {
public:
	typedef std::function<int (size_t& writtenCnt, const uint8_t* buf, size_t cnt)> FAshDriverWriteFunc;    /*!< Callback type for method registerSerialWriteFunc() */

	AshDriver() = delete; /* Construction without arguments is not allowed */

	/**
	 * @brief Constructor
	 * 
	 * @param ipCb Callback invoked on ASH state change
	 * @param i_timer_builder Timer builder object used to generate timers
	 * @param serialReadObservable An optional observable object used to be notified about new incoming bytes received on the serial port (or nullptr to disable read)
	 */
	AshDriver(CAshCallback *ipCb, const NSSPI::TimerBuilder& i_timer_builder, NSSPI::GenericAsyncDataInputObservable* serialReadObservable = nullptr);

	AshDriver(const AshDriver&) = delete; /* No copy construction allowed */

	/**
	 * @brief Destructor
	 */
	~AshDriver();

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
	 * @brief Set the serial async observable that will notify us of new incoming ASH bytes
	 * 
	 * @param serialReadObservable An optional observable object used to be notified about new incoming bytes received on the serial port (or nullptr to disable read)
	 */
	void registerSerialReadObservable(NSSPI::GenericAsyncDataInputObservable* serialReadObservable);

	/**
	 * @brief Check if a serial writer functor is registered
	 *
	 * @return true if a serial writer functor is active or false otherwise
	 */
	bool hasARegisteredSerialWriter() const;

	/**
	 * @brief Callback invoked by observable on received bytes (part of the IAsyncDataInputObserver interface)
	 * @param dataIn The pointer to the incoming bytes buffer
	 * @param dataLen The size of the data to read inside dataIn
	 */
	void handleInputData(const unsigned char* dataIn, const size_t dataLen);

	/**
	 * @brief Send an ASH frame via the registered serial port writer functor
	 *
	 * @return true if the frame could be sent
	 * 
	 * @note If no serial writer functor is registered, this method will return false
	 */
	bool sendAshFrame(const NSSPI::ByteBuffer& frame);

	/**
	 * @brief Send an ASH NCP reset frame
	 * 
	 * This frame must be sent to re-initialize the communication with the NCP when an ASH connection is initiated, so that any previous communication state is cancelled
	 * 
	 * @return true If the NCP reset frame was sent successfully (note that when we return true, we don't have any response or acknowledgment yet)
	 */
	bool sendResetNCPFrame(void);

	/**
	 * @brief Send an ASH ack frame
	 * 
	 * The sequence number will be computed to match the last succesfully received frame
	 * 
	 * @return true If the ack frame was sent successfully
	 */
	bool sendAckFrame(void);

	/**
	 * @brief Send an ASH data frame
	 * 
	 * @param[in] i_data The data payload of the frame we are sending
	 * 
	 * @return true If the data frame was sent successfully (note that when we return true, we don't have any response or acknowledgment yet)
	 */
	bool sendDataFrame(const NSSPI::ByteBuffer& i_data);

	/**
	 * @brief Get the current ASH connection state
	 * 
	 * @return true if ASH is in connected state
	 */
	bool isConnected() const;

protected:
	/**
	 * @brief Append a new chunk of incoming ASH bytes and try to decode the current accumulated bytes into an EZSP message
	 * 
	 * @param[in] i_data The new incoming ASH bytes
	 * 
	 * @note If an EZSP message could be extracted out of an ASH DATA frame, then our observers will be pushed a notification containing the extracted EZSP payload
	 */
	void decode(NSSPI::ByteBuffer& i_data);

	/**
	 * @brief Internal callback invoked when timeouts occur
	 * 
	 * @param[in] triggeringTimer The timer that timed out
	 */
	void trigger(NSSPI::ITimer* triggeringTimer);

private:
	std::unique_ptr<NSSPI::ITimer> ackTimer;	/*!< A timer checking acknowledgement of the initial RESET (if !stateConnected) of the last ASH DATA frame (if stateConnected) */
	NSEZSP::AshCodec ashCodec;	/*!< ASH codec utility methods */
	NSSPI::GenericAsyncDataInputObservable* serialReadObservable;	/*!< The observable object used to be notified about new incoming bytes received on the serial port */
	FAshDriverWriteFunc serialWriteFunc;   /*!< A function to write bytes to the serial port */
};

} // namespace NSEZSP
