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
	deliveredReadBytesCount(0),
	writtenBytesCount(0) { }

MockUartDriver::~MockUartDriver() {
	this->destroyAllScheduledIncomingChunks();
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
	else {
		writtenCnt = 0;
	}
	this->writtenBytesCount += writtenCnt;
	this->lastWrittenBytesTimestamp = now;
	return result;
}

void MockUartDriver::scheduleIncomingChunk(const struct MockUartScheduledByteDelivery& scheduledBytes) {
	
	bool frontSchedule;	/*!< Was the queued chunk list empty before scheduling these new scheduledBytes? If so, we need to start a new thread. */
	{
		std::lock_guard<std::mutex> lock(this->scheduledReadQueueMutex);
		frontSchedule = this->scheduledReadQueue.empty();	/* Are the scheduledBytes the only ones in the queue? */
		this->scheduledReadQueue.push(scheduledBytes);
		this->scheduledReadBytesCount += scheduledBytes.byteBuffer.size();
	}	/* scheduledReadQueueMutex released here */
	if (frontSchedule) {
		if (this->readBytesThread.joinable())
			this->readBytesThread.join();	/* Join any previously existing thread before creating a new one */
		this->readBytesThread = std::thread([this,scheduledBytes]() {
			std::chrono::milliseconds delay = scheduledBytes.delay;
			std::this_thread::sleep_for(delay);
			size_t rdcnt = 0;
			struct MockUartScheduledByteDelivery nextBytes;
			{
				std::lock_guard<std::mutex> lock(this->scheduledReadQueueMutex);
				//std::unique_lock<std::mutex> lock(this->scheduledReadQueueMutex);
				nextBytes = this->scheduledReadQueue.front();
				this->scheduledReadQueue.pop();	/* Discard these bytes as they are going to be used */
				rdcnt = nextBytes.byteBuffer.size();
				this->scheduledReadBytesCount -= rdcnt;
				this->deliveredReadBytesCount += rdcnt;
			} /* scheduledReadQueueMutex released here, but we copied the front of the queue inside variable nextBytes */
			if (dataInputObservable != nullptr) {
				std::unique_ptr<unsigned char[]> readData(new unsigned char[rdcnt]());	/* readData buffer will be deallocated when going our of scope */
				memcpy(readData.get(), &(nextBytes.byteBuffer[0]), rdcnt);	/* Fill-in buffer readData with the appropriate bytes */
				this->dataInputObservable->notifyObservers(readData.get(), rdcnt);	/* Notify observers */
			}
		});
	}
}

void MockUartDriver::destroyAllScheduledIncomingChunks() {
	bool queueIsEmpty;
	{
		std::lock_guard<std::mutex> lock(this->scheduledReadQueueMutex);
		queueIsEmpty = this->scheduledReadQueue.empty();
		if (!queueIsEmpty) {
			this->scheduledReadQueue = std::queue<struct MockUartScheduledByteDelivery>();	/* Replace the queue with a brand new (empty) one */
			this->scheduledReadBytesCount = 0;	/* No more byte queued */
		}
	}	/* scheduledReadQueueMutex released here */
	if (!queueIsEmpty) {	/* Kill the secondary thread */

	}
	if (this->readBytesThread.joinable()) {
		this->readBytesThread.join();
		this->readBytesThread = std::thread();	/* Destroy the secondary thread */
	}
}

size_t MockUartDriver::getScheduledIncomingChunksCount() {
	return this->scheduledReadQueue.size();/* FIXME: add mutex here */
}

size_t MockUartDriver::getScheduledIncomingBytesCount() {
	return this->scheduledReadBytesCount;/* FIXME: add mutex here */
}

size_t MockUartDriver::getDeliveredIncomingBytesCount() {
	return this->deliveredReadBytesCount;/* FIXME: add mutex here */
}

size_t MockUartDriver::getWrittenBytesCount() {
	return this->writtenBytesCount;
}

void MockUartDriver::close() {
}
