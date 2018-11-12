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
	if (started)
		return false;

	if (!callBackFunction)
		return false;

	duration = timeout;
	if (duration == 0) {
		callBackFunction(this);
	}
	else {
		auto tcb = [this,timeout](pp::Selector::TimedCbHandle&) {
			PPD_DEBUG("Timeout reached after %u", timeout);
			PPD_DEBUG("Now running timer's callback");
			callBackFunction(this);
		};
		this->m_eventLoop.getSelector().addCallback(m_toutcbhandle, duration, pp::Selector::ONCE, tcb);
	}
	return true;
}

bool RaritanTimer::stop() {
	if (!started)
		return false;
	this->m_toutcbhandle.removeFromSelector();
	duration=0;
	return true;
}

bool RaritanTimer::isRunning() {
	return started;
}
