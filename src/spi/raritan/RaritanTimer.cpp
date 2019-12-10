/**
 * @file RaritanTimer.cpp
 *
 * @brief Concrete implementation of ITimer using the Raritan framework
 */

#include "RaritanTimer.h"
#include "spi/GenericLogger.h"

RaritanTimer::RaritanTimer(RaritanEventLoop& eventLoop) : m_eventLoop(eventLoop), m_toutcbhandle() {
}

RaritanTimer::~RaritanTimer() {
	this->stop();
}

bool RaritanTimer::start(uint16_t timeout, std::function<void (ITimer* triggeringTimer)> callBackFunction) {
	plogD("Starting timer %p for %ums", this, timeout);

	if (started) {
		plogD("First stopping the already existing timer %p before starting again", this);
		this->stop();
	}

	if (!callBackFunction) {
		clogW << "Invalid callback function provided during start()\n";
		return false;
	}

	duration = timeout;
	if (duration == 0) {
		clogD << "Timeout set to 0, directly running callback function\n";
		callBackFunction(this);
	}
	else {
		auto tcb = [this,timeout,callBackFunction](pp::Selector::TimedCbHandle&) {
			plogD("Timeout reached after %u", timeout);
			plogD("Now running %p timer's callback", this);
			callBackFunction(this);
		};
		m_eventLoop.getSelector().addCallback(m_toutcbhandle, duration, pp::Selector::ONCE, tcb);
		started=true;
	}
	return true;
}

bool RaritanTimer::stop() {
	plogD("Stopping timer %p", this);
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
