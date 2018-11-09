#pragma once

#include <cstdint>

/**
 */
class ITimer {
public:
	ITimer();
	virtual ~ITimer();
	/**
	 * @brief Start a timer, run a callback after expiration of the configured time
	 *
	 * @param timeout The timeout (in ms)
	 * @param callBack The function to call at expiration of the timer
	 */
	virtual bool start(uint16_t timeout, ITimerCallback* callBack) = 0;

	/**
	 * @brief Stop and reset the timer
	 */
	virtual bool stop() = 0;

	/**
	 * @brief Is the timer currently running
	 *
	 * @return true if the timer is running
	 */
	virtual boot isRunning() = 0;

	/**
	 * @brief Get the remaining time to run the timer
	 */
	virtual uint16_t getRemaining() = 0;

protected:
	bool started;	/*!< Is the timer currently running */
	uint16_t duration;	/*!<The full duration of the timer (initial value if it is currently running) */
};
