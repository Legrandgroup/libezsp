/**
 * @file ITimerBuilder.h
 *
 * @brief Abstract interface to which must conforms implementations of factory classes that generate ITimer objects
 *
 * Used as a dependency inversion paradigm
 */
#pragma once

#include "ITimer.h"
#include <memory>	// For std::unique_ptr

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

/**
 * @brief Abstract factory to create ITimer instances
 */
class ITimerBuilder {
public:
	/**
	 * @brief Destructor
	 */
	virtual ~ITimerBuilder() { };

	/**
	 * @brief Create a new ITimer instance
	 *
	 * @return The new instance allocated
	 */
	virtual std::unique_ptr<ITimer> create() const = 0;
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
