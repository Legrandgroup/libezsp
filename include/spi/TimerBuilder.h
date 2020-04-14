/**
 * @file TimerBuilder.h
 *
 * @brief Builder returning a concrete implementation of ITimer objects (matching with the architecture)
 */
#ifndef __TIMERBUILDER_H__
#define __TIMERBUILDER_H__

#include <memory>

#include <ezsp/export.h>

#ifdef USE_RARITAN
#include <pp/Selector.h>
#endif

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
	TimerBuilder();

/* Note: Raritan environment has an extra constructor allowing to pass the event loop selector when constructing this builder */
#ifdef USE_RARITAN
	/**
	 * @brief Constructor using a specified selector
	 * 
	 * @param[in] selector The selector to use in the generated timer
	 */
	TimerBuilder(pp::Selector& selector);
#endif

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
#ifdef USE_RARITAN
	pp::Selector& eventSelector;	/*!< The raritan event selector */
#endif
};

} // namespace NSSPI

#endif	// __TIMERBUILDER_H__
