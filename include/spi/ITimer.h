/**
 * @file ITimer.h
 *
 * @brief Abstract interface to which must conforms implementations of classes that handle timed callbacks
 *
 * Used as a dependency inversion paradigm
 */

#pragma once

#include <cstdint>
#include <functional> // For std::function

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

namespace NSSPI {

class ITimer;

class ITimerVisitor {
protected:
	friend class ITimer;
	virtual void trigger(ITimer* triggeringTimer) = 0;
};

/**
 * @brief Abstract class to execute a callback after a given timeout
 */
class ITimer {
public:
	/**
	 * @brief Default constructor
	 */
	ITimer() : started(false), duration(0) { }

	/**
	 * @brief Destructor
	 */
	virtual ~ITimer() = default;

	/**
	 * @brief Start a timer, run a callback after expiration of the configured time
	 *
	 * @param timeout The timeout (in ms)
	 * @param callBackFunction The function to call at expiration of the timer (should be of type void f(ITimer*)) where argument will be a pointer to this timer object that invoked the callback
	 */
	virtual bool start(uint16_t timeout, std::function<void (ITimer* triggeringTimer)> callBackFunction) = 0;

	/**
	 * @brief Start a timer, run a callback after expiration of the configured time
	 *
	 * @param timeout The timeout (in ms)
	 * @param visitor The object to call at expiration.
	 */
	virtual bool start(uint16_t timeout, ITimerVisitor *visitor) {
		this->visitor = visitor;
		this->start(timeout, trigg);
	}

	/**
	 * @brief Stop and reset the timer
	 *
	 * @return true if we actually could stop a running timer
	 */
	virtual bool stop() = 0;

	/**
	 * @brief Is the timer currently running?
	 *
	 * @return true if the timer is running
	 */
	virtual bool isRunning() = 0;


protected:
	bool started;	/*!< Is the timer currently running */
	friend class ITimerVisitor;
	ITimerVisitor *visitor;
	static void trigg(ITimer* triggeringTimer) {
		triggeringTimer->visitor->trigger(triggeringTimer);
	}

public:
	uint16_t duration;	/*!<The full duration of the timer (initial value if it is currently running) */
};

} // namespace NSSPI

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
