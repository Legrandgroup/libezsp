/**
 * @file TimerBuilder.h
 *
 * @brief Concrete implementation of a ITimer factory
 */
#ifndef __TIMERBUILDER_H__
#define __TIMERBUILDER_H__

#include <memory>
#include "spi/ITimer.h"

namespace NSSPI {

/**
 * @brief Factory class to generate RaritanTimer objects
 */
class TimerBuilder {
public:
	/**
	 * @brief Constructor
	 */
	TimerBuilder() = default;

	/**
	 * @brief Destructor
	 */
	~TimerBuilder() = default;

	/**
	 * @brief Create a new timer
	 *
	 * @return The new timer created
	 */
	std::unique_ptr<ITimer> create() const;
private:
};

} // namespace NSSPI

#endif
