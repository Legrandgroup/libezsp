#include "RaritanTimer.h"

RaritanTimer::RaritanTimer() : m_eventLoop(nullptr) {
}

RaritanTimer::RaritanTimer(RaritanEventLoop* eventLoop) : m_eventLoop(eventLoop) {
}

RaritanTimer::~RaritanTimer() {
	this->stop();
}

bool RaritanTimer::start(uint16_t timeout, std::function<void (ITimer* triggeringTimer)> callBackFunction) {
	if (!m_eventLoop)
		return false;
	if (started)
		return false;
	this->callBack = callBackFunction;
	/* TODO: implement the launch of timer using Raritan framework */
	if (timeout==0)
		callBack(this);
	/* TODO */
	return false;
}

bool RaritanTimer::stop() {
	if (!m_eventLoop)
		return false;
	if (!started)
		return false;
	/* TODO: implement the stop of timer using Raritan framework */
	callBack=nullptr;
	return false;
}

uint16_t RaritanTimer::getRemaining() {
	/* TODO: not implemented */
	return 0;
}
