/**
 * @file RaritanTimer.h
 *
 * @brief Concrete implementation of ITimer using the Raritan framework
 */

#pragma once

#include <pp/Selector.h>

#include "spi/ITimer.h"
#include "spi/Logger.h"

namespace NSSPI {

/**
 * @brief Concrete implementation of ITimer using the Raritan framework
 */
class RaritanTimer : public ITimer {
public:
	/**
	 * @brief Constructor
	 *
	 * @param selector An optional selector instance to use to run timers
	 */
	RaritanTimer(pp::Selector& selector = *pp::SelectorSingleton::getInstance());

	/**
	 * @brief Destructor
	 */
	virtual ~RaritanTimer();

	/**
	 * @brief Start a timer, run a callback after expiration of the configured time
	 *
	 * @param timeout The timeout (in ms)
	 * @param callBackFunction The function to call at expiration of the timer (should be of type void f(ITimer*)) where argument will be a pointer to this timer object that invoked the callback
	 */
	bool start(uint32_t timeout, std::function<void (ITimer* triggeringTimer)> callBackFunction);

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


private:
	bool started;	/*!< Is the timer currently running */
	pp::Selector& m_eventSelector;	/*!< The raritan event selector */
	pp::Selector::TimedCbHandle m_toutcbhandle;	/*!< A handle on the callback */
};

} // namespace NSSPI
