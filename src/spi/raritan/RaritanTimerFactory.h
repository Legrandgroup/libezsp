#pragma once

#include "../ITimerFactory.h"
#include "RaritanTimer.h"
#include "RaritanEventLoop.h"

class RaritanTimerFactory : public ITimerFactory {
public:
	RaritanTimerFactory(RaritanEventLoop& eventLoop) : m_eventLoop(eventLoop) { }
	ITimer* create() {
		return new RaritanTimer;
	}
private:
	RaritanEventLoop& m_eventLoop;
};
