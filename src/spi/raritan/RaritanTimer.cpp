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

	callBack = callBackFunction;
	duration = timeout;
	if (duration == 0) {
		callBack(this);
	}
	else {
		auto tcb = [this,timeout](pp::Selector::TimedCbHandle&) {
			PPD_DEBUG("Timeout reached after %u", timeout);
			if (callBack) {
				PPD_DEBUG("Now running timer's callback");
				callBack(this);
			}
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
	callBack=nullptr;
	return true;
}

uint16_t RaritanTimer::getRemaining() {
	/* TODO: not implemented */
	return duration/2;	/* FIXME: Fake */
}
