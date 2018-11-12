#pragma once

#include "../ITimerFactory.h"
#include "SerialTimer.h"

class SerialTimerFactory : public ITimerFactory {
public:
	SerialTimerFactory();
	~SerialTimerFactory();
	ITimer* create();
};
