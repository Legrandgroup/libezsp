/**
 * @file CppThreadsTimer.cpp
 *
 * @brief Concrete implementation of ITimer using C++11 threads
 */

#include "CppThreadsTimer.h"

#include <chrono>
using namespace NSSPI;

CppThreadsTimer::CppThreadsTimer() :  waitingThread(), cv(), cv_m() { }

CppThreadsTimer::~CppThreadsTimer() {
	this->stop();
}

bool CppThreadsTimer::start(uint16_t timeout, std::function<void (ITimer* triggeringTimer)> callBackFunction) {

	if (this->started) {
		return false;
	}

	if (!callBackFunction) {
		return false;
	}

	this->duration = timeout;
	if (duration == 0) {
		callBackFunction(this);
	}
	else {
		this->started = true;
		this->waitingThread = std::thread([=]() {
			std::unique_lock<std::mutex> lock(this->cv_m);
			this->cv.wait_for(lock, std::chrono::milliseconds(timeout), [this]{return !this->started;});
			callBackFunction(this);
		});
	}

	return true;
}

bool CppThreadsTimer::stop() {

	if (! this->started) {
		return false;
	}
	if (this->started) {
		this->started = false;
		this->cv.notify_one();
	}
	if (this->waitingThread.joinable()) {
		this->waitingThread.join();
	}
	this->duration = 0;
	return true;
}

bool CppThreadsTimer::isRunning() {
	return this->started;
}
