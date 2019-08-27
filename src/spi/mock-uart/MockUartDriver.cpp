/**
 * @file MockUartDriver.cpp
 *
 * @brief Concrete implementation of a simulated UART driver for unit tests
 */

#include "MockUartDriver.h"

#include <exception>
#include <cstring>	// For memcpy()
#include <iostream>	// FIXME: for std::cerr during debug

MockUartDriver::MockUartDriver(std::function<int (size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta)> onWriteCallback) :
	readBytesThread(),
	scheduledReadQueueMutex(),
	scheduledReadQueue(),
	writeMutex(),
	dataInputObservable(nullptr),
	onWriteCallback(onWriteCallback),
	lastWrittenBytesTimestamp(std::chrono::time_point<std::chrono::high_resolution_clock>::min()),
	scheduledReadBytesCount(0),
	deliveredReadBytesCountMutex(),
	deliveredReadBytesCount(0) { }

MockUartDriver::~MockUartDriver() {
}

void MockUartDriver::setIncomingDataHandler(GenericAsyncDataInputObservable* uartIncomingDataHandler) {
	this->dataInputObservable = uartIncomingDataHandler;
}

int MockUartDriver::open(const std::string& serialPortName, unsigned int baudRate) {
	return 0;
}

int MockUartDriver::write(size_t& writtenCnt, const void* buf, size_t cnt) {
	
	std::lock_guard<std::recursive_mutex> lock(writeMutex);	/* Make sure there is only one simultaneous executiong of method write() */
	std::chrono::time_point<std::chrono::high_resolution_clock> now =  std::chrono::high_resolution_clock::now();
	int result = 0;
	if (this->onWriteCallback != nullptr) {
		std::chrono::duration<double, std::milli> delta;
		if (this->lastWrittenBytesTimestamp == std::chrono::time_point<std::chrono::high_resolution_clock>::min())	{ /* If we don't have any previous timestamp (these are the first bytes ever written)... */
			delta = std::chrono::duration<double, std::milli>::max();	/* ... set duration to maximum */
		}
		else {
			delta = now - this->lastWrittenBytesTimestamp;	/* ... otherwise compute the elapsed time since last bytes were written and store it in variable delta */
		}
		result = this->onWriteCallback(writtenCnt, buf, cnt, delta);	/* Invoke callback */
	}
	this->lastWrittenBytesTimestamp = now;
	return result;
}

void MockUartDriver::scheduleIncoming(struct MockUartScheduledByteDelivery&& scheduledBytes) {
	
	std::lock_guard<std::mutex> lock(this->scheduledReadQueueMutex);
	bool singleSchedule = this->scheduledReadQueue.empty();	/* Are the scheduledBytes the only ones in the queue? */
	this->scheduledReadQueue.push(scheduledBytes);
	if (singleSchedule) {
		this->readBytesThread = std::thread([this,scheduledBytes]() {
			std::this_thread::sleep_for(scheduledBytes.delay);
			std::unique_lock<std::mutex> lock(this->scheduledReadQueueMutex);
			if (dataInputObservable != nullptr) {
				size_t rdcnt = scheduledBytes.byteBuffer.size();
				std::unique_ptr<unsigned char[]> readData(new unsigned char[rdcnt]());	/* readData buffer will be deallocated when going our of scope */
				memcpy(readData.get(), &(scheduledBytes.byteBuffer[0]), rdcnt);	/* Fill-in buffer readData with the appropriate bytes */
				this->dataInputObservable->notifyObservers(readData.get(), rdcnt);	/* Notify observers */
			}
		});
	}
}

void MockUartDriver::close() {
}
