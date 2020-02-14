/**
 * @file MockUartDriver.h
 *
 * @brief Concrete implementation of a simulated UART driver for unit tests
 */

#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <functional>

#include "spi/IUartDriver.h"
#include "spi/ByteBuffer.h"

namespace NSSPI {

/**
 * @brief Structure to interact with a UART using libserialcpp
 */
class MockUartScheduledByteDelivery {
public:
	/**
	 * @brief Default constructor
	 */
	MockUartScheduledByteDelivery(const NSSPI::ByteBuffer& scheduledBuffer=NSSPI::ByteBuffer(), const std::chrono::milliseconds& scheduleDelay=std::chrono::milliseconds(0));

	/* Member variables */
	std::chrono::milliseconds delay;	/*!< A delay (in ms) to wait before making the bytes (stored in byteBuffer) available on the emulated UART */
	NSSPI::ByteBuffer byteBuffer;	/*!< The content of the emulated bytes */
};

/**
 * @brief Class to implement an emulated (robotised) UART for unit testing
 */
class MockUartDriver : public IUartDriver {
public:
	typedef std::function<int (size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta)> FWriteCallback; /*!< Callback type provided as argument to out constructor */

	/**
	 * @brief Default constructor
	 *
	 * @param onWriteCallback callback function to invoke when bytes are written to the emulated UART. This function should take 4 arguments:
	 *        size_t& writtenCnt: a reference where we will store the number of actually written bytes
	 *        const void* buf: a pointer to the buffer containing bytes to write
	 *        size_t cnt: the number of bytes to write
	 *        std::chrono::duration<double, std::milli> delta: the elapsed time since last bytes were written (in ms)
	 *        This callback should return 0 on success, errno on failure
	 */
	MockUartDriver(FWriteCallback onWriteCallback = nullptr);

	/**
	 * @brief Destructor
	 */
	~MockUartDriver();

	/**
	 * @brief Copy constructor
	 *
	 * Copy construction is forbidden on this class
	 */
	MockUartDriver(const MockUartDriver& other) = delete;

	/**
	 * @brief Assignment operator
	 *
	 * Copy construction is forbidden on this class
	 */
	MockUartDriver& operator=(const MockUartDriver& other) = delete;

	/**
	 * @brief Set the incoming data handler (a derived class of GenericAsyncDataInputObservable) that will notify observers when new bytes are available on the UART
	 *
	 * @param uartIncomingDataHandler A pointer to the new handler (the eventual previous handler that might have been set at construction will be dropped)
	 */
	void setIncomingDataHandler(GenericAsyncDataInputObservable* uartIncomingDataHandler);

	/**
	 * @brief Get the currently active incoming data handler (a derived class of GenericAsyncDataInputObservable) that notifies observers
	 *
	 * @return A pointer to the current GenericAsyncDataInputObservable handler (might also be nullptr)
	 */
	GenericAsyncDataInputObservable* getIncomingDataHandler() const;

	/**
	 * @brief Opens the serial port
	 *
	 * @param serialPortName The name of the serial port to open (eg: "/dev/ttyUSB0")
	 * @param baudRate The baudrate to enforce on the serial port
	 *
	 * @return 0 on success, errno on failure
	 *
	 * This method is purely virtual and should be overridden by inheriting classes defining a concrete implementation
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
	 * @brief Schedule a byte sequence to be ready for read on emulated serial port
	 *
	 * @note Attribute scheduledBytes.delay specifies the delay between this read and the previous read.
	 *       Thus if this new chunk is the only one in the queue, the delay is relative to now.
	 *       If this new chunk is queue behind one or more existing chunks, the delay is relative to the execution (read) of the previous chunk
	 *
	 * @param scheduledBytes The buffer to queue for future emulate reads
	 */
	void scheduleIncomingChunk(const MockUartScheduledByteDelivery& scheduledBytes);

	/**
	 * @brief Get a string representation of the scheduled incoming chunks (for debug)
	 *
	 * @return The current scheduled queue, for example "[00 01 af, 85 63]" for a queue that would contain a first chunk of 3 bytes, followed by a chunk of 2 bytes
	 */
	std::string scheduledIncomingChunksToString();

	/**
	 * @brief Remove all potential byte sequences that were scheduled to be ready for read on emulated serial port (but were not yet sent out)
	 *
	 * @note The bytes that were still in the queue (if any) will disappear
	 */
	void destroyAllScheduledIncomingChunks();

	/**
	 * @brief Get the number of chunks currently in the emulated serial port schedule
	 *
	 * @return The number of chunks in the emulated serial port schedule queue
	 */
	size_t getScheduledIncomingChunksCount();

	/**
	 * @brief Get the number of bytes currently in the emulated serial port schedule
	 *
	 * @return The number of bytes in the emulated serial port schedule queue
	 */
	size_t getScheduledIncomingBytesCount();

	/**
	 * @brief Get the number of bytes read from the emulated serial port so far (cumulative)
	 *
	 * @return The total number of bytes delivered so far by the emulated serial port
	 */
	size_t getDeliveredIncomingBytesCount();

	/**
	 * @brief Get the number of bytes announced to have been written by the onWriteCallback function
	 *
	 * @return The number of bytes written to the emulated serial port (computed as the total sum of the onWriteCallback function's successive writtenCnt returned values)
	 */
	size_t getWrittenBytesCount();

	/**
	 * @brief Close the serial port
	 */
	void close();

private:
	std::thread readBytesThread;	/*!< The thread that will generate emulated read bytes prepared in scheduledReadQueue */
	std::mutex scheduledReadQueueMutex;	/*!< A mutex to handle access to scheduledReadQueue, scheduledReadBytesCount or deliveredReadBytesCount */
	std::queue<struct MockUartScheduledByteDelivery> scheduledReadQueue;	/*!< The scheduled read bytes queue. Grab scheduledReadQueueMutex before accessing this */
public:
	std::recursive_mutex writeMutex;	/*!< Mutex to protect writes... recursive to allow the caller to grab the mutex for us */
private:
	GenericAsyncDataInputObservable *dataInputObservable;		/*!< The observable that will notify observers when new bytes are available on the UART */
	FWriteCallback onWriteCallback;	/*!< Callback invoked each time bytes are written to the emulated UART, this callback must have a prototype that takes 3 parameters: size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta. delta being the time since last bytes were written (in ms) */
	std::chrono::time_point<std::chrono::high_resolution_clock> lastWrittenBytesTimestamp;	/*!< A timestamp of the last bytes written */
	size_t scheduledReadBytesCount;	/*!< The current size of the scheduled read bytes queue. Grab scheduledReadQueueMutex before accessing this  */
	size_t deliveredReadBytesCount;	/*!< The cumulative number of emulated read bytes delivered to the GenericAsyncDataInputObservable observer since the instanciation of this object. Grab scheduledReadQueueMutex before accessing this */
	size_t writtenBytesCount;	/*!< The number of bytes written, as a total sum of the onWriteCallback function's successive writtenCnt returned values */
};

} // namespace NSSPI
