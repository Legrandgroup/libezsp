/**
 * @file RaritanTimerFactory.cpp
 */

#include "RaritanTimerFactory.h"
#include "RaritanTimer.h"

RaritanTimerFactory::RaritanTimerFactory(RaritanEventLoop& eventLoop) : m_eventLoop(eventLoop) {

}

RaritanTimerFactory::~RaritanTimerFactory() {

}

std::unique_ptr<ITimer> RaritanTimerFactory::create() const {
	return new RaritanTimer(m_eventLoop);
}
