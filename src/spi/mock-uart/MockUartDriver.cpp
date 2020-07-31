/**
 * @file MockUartDriver.cpp
 *
 * @brief Concrete implementation of a simulated UART driver for unit tests
 */

#include "MockUartDriver.h"

#include "spi/ILogger.h"
#include <exception>
#include <cstring>	// For memcpy()
#include <sstream>	// For std::stringstream
#include <iomanip>	// For std::setw etc.
#include <iostream>	// FIXME: for std::cerr during debug

#include "spi/GenericAsyncDataInputObservable.h"
using NSSPI::MockUartDriver;
using NSSPI::MockUartScheduledByteDelivery;
using NSSPI::GenericAsyncDataInputObservable;

MockUartScheduledByteDelivery::MockUartScheduledByteDelivery(const NSSPI::ByteBuffer& scheduledBuffer, const std::chrono::milliseconds& scheduleDelay) :
	delay(scheduleDelay),
	byteBuffer(scheduledBuffer) { }

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

GenericAsyncDataInputObservable* MockUartDriver::getIncomingDataHandler() const {
	return this->dataInputObservable;
}

int MockUartDriver::open(const std::string& serialPortName, unsigned int baudRate) {
	return 0;
}

int MockUartDriver::write(size_t& writtenCnt, const uint8_t* buf, size_t cnt) {

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

void MockUartDriver::scheduleIncomingChunk(const MockUartScheduledByteDelivery& scheduledBytes) {

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
			bool terminateReadSchedulingThread = false;
			struct MockUartScheduledByteDelivery nextChunk;
			while(!terminateReadSchedulingThread) {
				{
					std::lock_guard<std::mutex> lock(this->scheduledReadQueueMutex);
					if (this->scheduledReadQueue.empty()) {	/* There are still chunks to process in the queue, reschedule this thread */
						terminateReadSchedulingThread = true;	/* No more chunk to process->we will terminate the current thread */
						break;
					}
					nextChunk = this->scheduledReadQueue.front();
					this->scheduledReadQueue.pop();	/* Discard these bytes as they are going to be used */
				} /* scheduledReadQueueMutex released here, but we copied the front of the queue inside variable nextChunk */
				std::this_thread::sleep_for(nextChunk.delay);
				size_t rdcnt = nextChunk.byteBuffer.size();
				{
					std::lock_guard<std::mutex> lock(this->scheduledReadQueueMutex);
					this->scheduledReadBytesCount -= rdcnt;
					this->deliveredReadBytesCount += rdcnt;
				} /* scheduledReadQueueMutex released here */
				if (dataInputObservable != nullptr) {
					unsigned char readData[rdcnt];
					memcpy(readData, &(nextChunk.byteBuffer[0]), rdcnt);	/* Fill-in buffer readData with the appropriate bytes */
					this->dataInputObservable->notifyObservers(readData, rdcnt);	/* Notify observers */
				}
			}
		});
	}
}

std::string MockUartDriver::scheduledIncomingChunksToString() {
	std::stringstream result;
	std::queue<struct MockUartScheduledByteDelivery> scheduledReadQueueCopy;
	{
		std::lock_guard<std::mutex> lock(this->scheduledReadQueueMutex);
		scheduledReadQueueCopy = this->scheduledReadQueue;	/* Copy this->scheduledReadQueue to be able to iterate over its elements */
	} /* scheduledReadQueueMutex released here */

	struct MockUartScheduledByteDelivery nextChunk;

	while (!scheduledReadQueueCopy.empty()) {
		if (result.str().length() != 0 )	/* result string is not empty (discarding the leading '[') */
			result << ", ";	/* Add a separator */
		nextChunk = scheduledReadQueueCopy.front();
		NSSPI::ByteBuffer bytes(nextChunk.byteBuffer);
		result << bytes;
		scheduledReadQueueCopy.pop();
	}
	return "[" + result.str() + "]";
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
