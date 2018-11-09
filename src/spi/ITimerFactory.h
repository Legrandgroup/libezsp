/**
 * @file ITimerFactory.h
 */
#pragma once

#include "ITimer.h"

/**
 * @class ITimerFactory
 *
 * @brief Abstract factory to create ITimer instances
 */
class ITimerFactory {
public:
	/**
	 * @brief Destructor
	 */
	virtual ~ITimerFactory() { };

	/**
	 * @brief Create a new ITimer instance
	 *
	 * @return The new instance allocated
	 */
	virtual ITimer* create() const = 0;
};
