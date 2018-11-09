#pragma once

#include "ITimer.h"

/**
 * Abstract factory defines methods to create all
 * related products.
 */
class ITimerFactory {
public:
	virtual ~ITimerFactory() { };
	virtual ITimer* create() const = 0;
};
