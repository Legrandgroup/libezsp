/**
 * @file CppThreadsTimer.h
 *
 * @brief Concrete implementation of ITimer using C++11 threads
 */

#pragma once

#include "spi/ITimer.h"

#include <thread>
#include <condition_variable>
namespace NSSPI {

/**
 * @brief Concrete implementation of ITimer using C++11 threads
 */
class CppThreadsTimer : public ITimer {
public:

	/**
	 * @brief Default constructor
	 */
	CppThreadsTimer();

	/**
	 * @brief Destructor
	 */
	virtual ~CppThreadsTimer();

	/**
	 * @brief Start a timer, run a callback after expiration of the configured time
	 *
	 * @param timeout The timeout (in ms)
	 * @param callBackFunction The function to call at expiration of the timer (should be of type void f(ITimer*)) where argument will be a pointer to this timer object that invoked the callback
	 */
	bool start(uint16_t timeout, TimerCallback callBackFunction) override;

	/**
	 * @brief Stop and reset the timer
	 *
	 * @return true if we actually could stop a running timer
	 */
	bool stop() final;

	/**
	 * @brief Is the timer currently running?
	 *
	 * @return true if the timer is running
	 */
	bool isRunning();

	/**
	 * @brief the thread routine
	 */
	void routine();

private:
	std::thread waitingThread;	/*!< The thread that will wait for the specified timeout and will then run the callback */
	std::condition_variable cv;	/*!< A condition variable that allows to unlock the wait performed by waitingThread (this allows stopping that secondary thread) */
	std::mutex cv_m;	/*!< A mutex to handle access to variable cv */
  TimerCallback callback;
};

} // namespace NSSPI
