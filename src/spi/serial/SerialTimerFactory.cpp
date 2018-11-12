#include "SerialTimerFactory.h"

SerialTimerFactory::SerialTimerFactory() { };

SerialTimerFactory::~SerialTimerFactory() { };

ITimer* SerialTimerFactory::create() {
	return new SerialTimer();
}
