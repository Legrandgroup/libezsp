#pragma once

#include "../ITimer.h"
#include "RaritanEventLoop.h"

/**
 * @brief Concrete implementation of ITimer using the Raritan framework
 */
class RaritanTimer : public ITimer {
public:
	RaritanTimer() = delete;
	RaritanTimer(RaritanEventLoop& eventLoop);
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
	 */
	bool stop();

	/**
	 * @brief Get the remaining time to run the timer
	 */
	uint16_t getRemaining();

private:
	RaritanEventLoop& m_eventLoop;
};
