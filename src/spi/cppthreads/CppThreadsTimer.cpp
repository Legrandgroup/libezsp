/**
 * @file CppThreadsTimer.cpp
 *
 * @brief Concrete implementation of ITimer using C++11 threads
 */

#include "CppThreadsTimer.h"
#include "spi/Logger.h"

#include <chrono>
using NSSPI::CppThreadsTimer;
using NSSPI::ITimer;

CppThreadsTimer::CppThreadsTimer() : started(false), waitingThread(), cv(), cv_m() { }

CppThreadsTimer::~CppThreadsTimer() {
	this->stop();
}

bool CppThreadsTimer::start(uint16_t timeout, NSSPI::TimerCallback callBackFunction) {
	//clogD << "Starting timer " << static_cast<void *>(this) << " for " << std::dec << static_cast<unsigned int>(timeout) << "ms\n";

	if (this->started) {
		clogD << "First stopping the already existing timer " << static_cast<void *>(this) << " before starting again\n";
		this->stop();
	}

	if (!callBackFunction) {
		clogW << "No callback function provided\n";
		return false;
	}

	this->duration = timeout;
	if (this->duration == 0) {
		clogD << "Timeout set to 0, directly running callback function\n";
		callBackFunction(this);
	}
	else {
		this->started = true;
		this->callback = callBackFunction;
		this->waitingThread = std::thread(&CppThreadsTimer::routine, this);
	}

	return true;
}

bool CppThreadsTimer::stop() {

	if (! this->started) {
		return false;
	}
	this->started = false;
	this->cv.notify_one();
	if (this->waitingThread.joinable()) {
		this->waitingThread.join();
	}
	this->duration = 0;
	return true;
}

bool CppThreadsTimer::isRunning() {
	return this->started;
}

void CppThreadsTimer::routine()
{
	std::unique_lock<std::mutex> lock(this->cv_m);
	this->cv.wait_for(lock, std::chrono::milliseconds(this->duration), [this]{return !this->started;});
	if (this->started) {
		this->callback(this);
	}
}
