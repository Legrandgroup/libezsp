/**
 * @file SerialTimer.cpp
 *
 * @brief Concrete implementation of ITimer using C++11 threads
 */

#include "SerialTimer.h"

#include <chrono>
#include <iostream>

SerialTimer::SerialTimer() :  waitingThread(), cv(), cv_m() { }

SerialTimer::~SerialTimer() {
	this->stop();
}

bool SerialTimer::start(uint16_t timeout, std::function<void (ITimer* triggeringTimer)> callBackFunction) {

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

bool SerialTimer::stop() {

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

bool SerialTimer::isRunning() {
	return this->started;
}

