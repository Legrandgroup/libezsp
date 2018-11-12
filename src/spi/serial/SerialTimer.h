#pragma once

#include "../ITimer.h"

#include <thread>
#include <condition_variable>

/**
 * @brief Concrete implementation of ITimer using the C++11
 */
class SerialTimer : public ITimer {
public:

	/**
	 * @brief Default constructor
	 */
	SerialTimer();

	/**
	 * @brief Destructor
	 */
	~SerialTimer();

	/**
	 * @brief Start a timer, run a callback after expiration of the configured time
	 *
	 * @param timeout The timeout (in ms)
	 * @param callBackFunction The function to call at expiration of the timer (should be of type void f(ITimer*)) where argument will be a pointer to this timer object that invoked the callback
	 */
	bool start(uint16_t timeout, std::function<void (ITimer* triggeringTimer)> callBackFunction);

	/**
	 * @brief Stop and reset the timer
	 *
	 * @return true if we actually could stop a running timer
	 */
	bool stop();

	/**
	 * @brief Is the timer currently running?
	 *
	 * @return true if the timer is running
	 */
	bool isRunning();

private:
	std::thread waitingThread;
	std::condition_variable cv;
	std::mutex cv_m;
};
