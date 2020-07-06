/**
 * @file ITimer.h
 *
 * @brief Abstract interface to which must conforms implementations of classes that handle timed callbacks
 *
 * Used as a dependency inversion paradigm
 */

#ifndef __ITIMER_H__
#define __ITIMER_H__

#include <cstdint>
#include <functional> // For std::function

#include <ezsp/export.h>

namespace NSSPI {

class ITimer;

class LIBEXPORT ITimerVisitor {
protected:
	friend class ITimer;
	virtual void trigger(ITimer* triggeringTimer) = 0;
	virtual ~ITimerVisitor() = default;
};

using TimerCallback = std::function<void (ITimer* triggeringTimer)>;

/**
 * @brief Abstract class to execute a callback after a given timeout
 */
class LIBEXPORT ITimer {
public:
	/**
	 * @brief Default constructor
	 */
	ITimer() : visitor(nullptr), duration(0) { }

	/**
	 * @brief Destructor
	 */
	virtual ~ITimer() = default;

	/**
	 * @brief Copy constructor
	 *
	 * @warning Copy construction is not allowed
	 */
	ITimer(const ITimer&) = delete;

	/**
	 * @brief Assignment operator
	 *
	 * @warning Assignment is not allowed
	 */
	ITimer& operator=(const ITimer&) = delete;

	/**
	 * @brief Start a timer, run a callback after expiration of the configured time
	 *
	 * @param timeout The timeout (in ms)
	 * @param callBackFunction The function to call at expiration of the timer (should be of type void f(ITimer*)) where argument will be a pointer to this timer object that invoked the callback
	 * 
	 * @return true if the timer was started successfully
	 */
	virtual bool start(uint32_t timeout, TimerCallback callBackFunction) = 0;

	/**
	 * @brief Start a timer, run a callback after expiration of the configured time
	 *
	 * @param timeout The timeout (in ms)
	 * @param visitor The object to call at expiration.
	 */
	virtual bool start(uint32_t timeout, ITimerVisitor *visitor) {
		this->visitor = visitor;
		return this->start(timeout, trigg);
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
	friend class ITimerVisitor;
	static void trigg(ITimer* triggeringTimer) {
		if (triggeringTimer != nullptr && triggeringTimer->visitor != nullptr) {
			triggeringTimer->visitor->trigger(triggeringTimer);
		}
	}

public:
	ITimerVisitor *visitor;	/*!< An external visitor class that will be invoked at timeout */
	uint32_t duration;	/*!<The full duration of the timer (initial value if it is currently running) */
};

} // namespace NSSPI

#endif	// __ITIMER_H__