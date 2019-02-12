/**
 * @file MockUartDriver.h
 *
 * @brief Concrete implementation of a simulated UART driver for unit tests
 */

#pragma once


#include "../IUartDriver.h"
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>

/**
 * @brief Structure to interact with a UART using libserialcpp
 */
struct MockUartScheduledByteDelivery {
	std::chrono::milliseconds delay;	/*!< A delay (in ms) to wait before making the bytes (stored in byteBuffer) available on the emulated UART */
	std::vector<unsigned char> byteBuffer;	/*!< The content of the emulated bytes */
};

/**
 * @brief Class to implement an emulated (robotised) UART for unit testing
 */
class MockUartDriver : public IUartDriver {
public:
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
	MockUartDriver(std::function<int (size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta)> onWriteCallback = nullptr);

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
	 * @brief Opens the serial port
	 *
	 * @param serialPortName The name of the serial port to open (eg: "/dev/ttyUSB0")
	 * @param baudRate The baudrate to enforce on the serial port
	 */
	void open(const std::string& serialPortName, unsigned int baudRate = 115200);

	/**
	 * @brief Write a byte sequence to the serial port
	 *
	 * @param[out] writtenCnt How many bytes were actually written
	 * @param[in] buf data buffer to write
	 * @param[in] cnt byte count of data to write
	 *
	 * @return 0 on success, errno on failure
	 */
	int write(size_t& writtenCnt, const void* buf, size_t cnt);

	/**
	 * @brief Schedule a byte sequence to be ready for read on emulated serial port
	 *
	 * @param scheduledBytes The buffer to queue for future emulate reads
	 */
	void scheduleIncoming(struct MockUartScheduledByteDelivery&& scheduledBytes);

	/**
	 * @brief Get the number of bytes currently in the emulated serial port schedule
	 *
	 * @return The number of bytes in the emulated serial port schedule queue
	 */
	size_t getScheduledIncomingCount();

	/**
	 * @brief Get the number of bytes read from the emulated serial port so far
	 *
	 * @param The number of bytes delivered so far by the emulated serial port
	 */
	size_t getDeliveredIncomingCount();

	/**
	 * @brief Close the serial port
	 */
	void close();

private:
	std::thread readBytesThread;	/*!< The thread that will generate emulated read bytes prepared in scheduledReadQueue */
	std::mutex scheduledReadQueueMutex;	/*!< A mutex to handle access to scheduledReadQueue */
	std::queue<struct MockUartScheduledByteDelivery> scheduledReadQueue;	/*!< The scheduled read bytes queue */
public:
	std::recursive_mutex writeMutex;	/* Mutex to protect writes... recursive to allow the caller to grab the mutex for us */
private:
	GenericAsyncDataInputObservable *dataInputObservable;		/*!< The observable that will notify observers when new bytes are available on the UART */
	std::function<int (size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta)> onWriteCallback;	/*!< Callback invoked each time bytes are written to the emulated UART, this callback must have a prototype that takes 3 parameters: size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta. delta being the time since last bytes were written (in ms) */
	std::chrono::time_point<std::chrono::high_resolution_clock> lastWrittenBytesTimestamp;	/*!< A timestamp of the last bytes written */
	size_t scheduledReadBytesCount;	/*!< The current size of the scheduled read bytes queue */
	std::mutex deliveredReadBytesCountMutex;	/*!< A mutex to handle access to deliveredReadBytesCount */
	size_t deliveredReadBytesCount;	/*!< The number of emulated read bytes delivered to the GenericAsyncDataInputObservable observer */
};
