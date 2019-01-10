#include "SerialTimerFactory.h"

SerialTimerFactory::SerialTimerFactory() { }

SerialTimerFactory::~SerialTimerFactory() { }

std::unique_ptr<ITimer> SerialTimerFactory::create() const {
	return std::unique_ptr<ITimer>(new SerialTimer());
}