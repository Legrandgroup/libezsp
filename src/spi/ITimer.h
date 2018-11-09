#pragma once

#include <cstdint>
#include <functional> // For std::function

/**
 */
class ITimer {
public:
	ITimer() : started(false), duration(0) { }
	virtual ~ITimer() { }

	/**
	 * @brief Start a timer, run a callback after expiration of the configured time
	 *
	 * @param timeout The timeout (in ms)
	 * @param callBackFunction The function to call at expiration of the timer (should be of type void f(ITimer*)) where argument will be a pointer to this timer object that invoked the callback
	 */
	virtual bool start(uint16_t timeout, std::function<void (ITimer* triggeringTimer)> callBackFunction) = 0;

	/**
	 * @brief Stop and reset the timer
	 */
	virtual bool stop() = 0;

	/**
	 * @brief Is the timer currently running
	 *
	 * @return true if the timer is running
	 */
	//virtual bool isRunning() = 0;

	/**
	 * @brief Get the remaining time to run the timer
	 */
	virtual uint16_t getRemaining() = 0;

public:
	bool started;	/*!< Is the timer currently running */
	uint16_t duration;	/*!<The full duration of the timer (initial value if it is currently running) */
	std::function<void (ITimer* triggeringTimer)> callBack;	/*!< The callback function that will be triggered by this timer */
};
