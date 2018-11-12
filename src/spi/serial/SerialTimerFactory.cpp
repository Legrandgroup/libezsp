#include "SerialTimerFactory.h"

SerialTimerFactory::SerialTimerFactory() { };

SerialTimerFactory::~SerialTimerFactory() { };

ITimer* SerialTimerFactory::create() const {
	return new SerialTimer();
}
