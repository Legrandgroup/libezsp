/**
 * @file RaritanTimer.h
 */

#pragma once

#include "../ITimer.h"
#include "RaritanEventLoop.h"

/**
 * @brief Concrete implementation of ITimer using the Raritan framework
 */
class RaritanTimer : public ITimer {
public:
	/**
	 * @brief Default constructor
	 *
	 * Construction without arguments is not allowed
	 */
	RaritanTimer() = delete;

	/**
	 * @brief Constructor
	 *
	 * @param eventLoop A RaritanEventLoop object to access the mainloop selector
	 */
	RaritanTimer(RaritanEventLoop& eventLoop);

	/**
	 * @brief Destructor
	 */
	~RaritanTimer();

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
	RaritanEventLoop& m_eventLoop;	/*!< The raritan mainloop */
	pp::Selector::TimedCbHandle m_toutcbhandle;	/*!< A handle on the callback */
};
