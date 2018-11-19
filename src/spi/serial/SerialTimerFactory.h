#pragma once

#include "../ITimerFactory.h"
#include "SerialTimer.h"

class SerialTimerFactory : public ITimerFactory {
public:
	SerialTimerFactory();
	~SerialTimerFactory();
	std::unique_ptr<ITimer> create() const;
};
