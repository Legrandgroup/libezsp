/**
 * @file RaritanTimer.cpp
 */

#include "RaritanTimer.h"

RaritanTimer::RaritanTimer(RaritanEventLoop& eventLoop) : m_eventLoop(eventLoop), m_toutcbhandle() {
}

RaritanTimer::~RaritanTimer() {
	this->stop();
}

bool RaritanTimer::start(uint16_t timeout, std::function<void (ITimer* triggeringTimer)> callBackFunction) {
	PPD_DEBUG("Starting timer %p for %dms", this, timeout);

	if (started)
		this->stop();

	if (!callBackFunction) {
		PPD_WARN("Invalid callback function provided during start()");
		return false;
	}

	duration = timeout;
	if (duration == 0) {
		PPD_DEBUG("Timeout set to 0, directly running callback function");
		callBackFunction(this);
	}
	else {
		auto tcb = [this,timeout,callBackFunction](pp::Selector::TimedCbHandle&) {
			PPD_DEBUG("Timeout reached after %u", timeout);
			PPD_DEBUG("Now running %p timer's callback", this);
			callBackFunction(this);
		};
		m_eventLoop.getSelector().addCallback(m_toutcbhandle, duration, pp::Selector::ONCE, tcb);
		started=true;
	}
	return true;
}

bool RaritanTimer::stop() {
	PPD_DEBUG("Stopping timer %p", this);
	if (!started) {
		PPD_WARN("Got a request to stop a timer that was not running");
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
