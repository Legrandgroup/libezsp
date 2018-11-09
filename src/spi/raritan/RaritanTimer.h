#pragma once

#include "../ITimer.h"
#include "RaritanEventLoop.h"

/**
 * @brief Concrete implementation of ITimer using the Raritan framework
 */
class RaritanTimer : public ITimer {
	RaritanTimer() : started(false), duration(0), eventLoop(nullptr);
	RaritanTimer(RaritanEventLoop* eventLoop): started(false), duration(0), eventLoop(eventLoop);
	~RaritanTimer() {
		this->stop();
	}
	bool start(uint32_t timeMs, ITimerCallback* callBack) {
		if (!m_eventLoop)
			return false;
		if (m_started)
			return false;
		/* TODO: implement the launch of timer using Raritan framework */
		return false;
	}
	bool stop() {
		if (!m_eventLoop)
			return false;
		if (!m_started)
			return false;
		/* TODO: implement the stop of timer using Raritan framework */
		return false;
	}
	uint32_t getRemaining() {
		/* TODO: not implemented */
		return 0;
	}
private:
	RaritanEventLoop* m_eventLoop;
};
