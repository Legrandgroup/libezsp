/**
 * @file SerialTimerFactory.h
 *
 * @brief Concrete implementation of a ITimer factory returning SerialTimer objects
 */
#pragma once

#include "../ITimerFactory.h"
#include "SerialTimer.h"

/**
 * @brief Factory class to generate SerialTimer objects
 */
class SerialTimerFactory : public ITimerFactory {
public:
	SerialTimerFactory();
	~SerialTimerFactory();
	std::unique_ptr<ITimer> create() const;
};
