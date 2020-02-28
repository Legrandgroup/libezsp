/**
 * @file TimerBuilder.h
 *
 * @brief Builder returning a concrete implementation of ITimer objects (matching with the architecture)
 */
#ifndef __TIMERBUILDER_H__
#define __TIMERBUILDER_H__

#include <memory>

#include <ezsp/export.h>
#include <spi/ITimer.h>

namespace NSSPI {

/**
 * @brief Utility class to generate ITimer objects
 */
class LIBEXPORT TimerBuilder {
public:
	/**
	 * @brief Default constructor
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

#endif	// __TIMERBUILDER_H__
