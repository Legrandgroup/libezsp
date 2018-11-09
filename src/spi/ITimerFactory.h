#pragma once

#include "ITimer.h"

/**
 * Abstract factory defines methods to create all
 * related products.
 */
class TimerFactory {
public:
	virtual ITimer* create() = 0;
};
