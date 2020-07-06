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
	 * 
	 * @return true if the timer was started successfully
	 */
	bool start(uint32_t timeout, NSSPI::TimerCallback callBackFunction) override;

	/**
	 * @brief Stop and reset the timer
	 *
	 * @note When invoking stop(), the callback associated with this timer will not be run
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

protected:
	/**
	 * @brief The routine that will run in a secondary thread
	 */
	void routine();

private:
	bool started;	/*!< Is the timer currently running */
	std::thread waitingThread;	/*!< The thread that will wait for the specified timeout and will then run the callback */
	std::condition_variable cv;	/*!< A condition variable that allows to unlock the wait performed by waitingThread (this allows stopping that secondary thread) */
	std::mutex cv_m;	/*!< A mutex to handle access to variable cv */
	TimerCallback callback;	/*!< The callback to invoke when the timer elapses */
};

} // namespace NSSPI
