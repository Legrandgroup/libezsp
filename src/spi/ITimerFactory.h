/**
 * @file ITimerFactory.h
 */
#pragma once

#include "ITimer.h"
#include <memory>	// For std::unique_ptr

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
	virtual std::unique_ptr<ITimer> create() const = 0;
};
