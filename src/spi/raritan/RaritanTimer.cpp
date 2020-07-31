/**
 * @file RaritanTimer.cpp
 *
 * @brief Concrete implementation of ITimer using the Raritan framework
 */

#include "RaritanTimer.h"
#include "spi/ILogger.h"

using NSSPI::RaritanTimer;
using NSSPI::ITimer;

RaritanTimer::RaritanTimer(pp::Selector& selector) :
	started(false),
	m_eventSelector(selector),
	m_toutcbhandle() {
}

RaritanTimer::~RaritanTimer() {
	this->stop();
}

bool RaritanTimer::start(uint32_t timeout, NSSPI::TimerCallback callBackFunction) {
	//clogD << "Starting timer " << static_cast<void *>(this) << " for " << std::dec << static_cast<unsigned int>(timeout) << "ms\n";

	if (this->started) {
		//clogD << "First stopping the already existing timer " << static_cast<void *>(this) << " before starting again\n";
		this->stop();
	}

	if (!callBackFunction) {
		clogW << "No callback function provided\n";
		return false;
	}

	this->duration = timeout;
	if (this->duration == 0) {
		clogD << "Timeout set to 0, directly running callback function\n";
		callBackFunction(this);
	}
	else {
		auto tcb = [this,timeout,callBackFunction](pp::Selector::TimedCbHandle&) {
			//clogD << "Timeout reached after " << std::dec << static_cast<unsigned int>(timeout) << "ms. Now running " << static_cast<void *>(this) << " timer's callback\n";
			callBackFunction(this);
		};
		this->started = true;
		m_eventSelector.addCallback(m_toutcbhandle, this->duration, pp::Selector::ONCE, tcb);
	}
	return true;
}

bool RaritanTimer::stop() {
	//plogD("Stopping timer %p", this);
	if (!started) {
		return false;
	}
	m_toutcbhandle.removeFromSelector();
	started=false;
	duration=0;
	return true;
}

bool RaritanTimer::isRunning() {
	return m_toutcbhandle.isCallbackPending();
}
