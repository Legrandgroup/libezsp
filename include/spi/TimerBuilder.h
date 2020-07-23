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
	class TimerBuilder; // // Forward declaration for swap() below
}

void swap(NSSPI::TimerBuilder& first, NSSPI::TimerBuilder& second); /* Declaration before qualifying ::swap() as friend for class NSSPI::TimerBuilder */

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

	/**
	 * \brief swap function to allow implementing of copy-and-swap idiom on instances of TimerBuilder
	 *
	 * This function will swap all attributes of \p first and \p second
	 * See http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
	 *
	 * \param first The first object
	 * \param second The second object
	 */
	friend void (::swap)(NSSPI::TimerBuilder& first, NSSPI::TimerBuilder& second);

private:
#ifdef USE_RARITAN
	pp::Selector& eventSelector;	/*!< The raritan event selector */
#endif
};

} // namespace NSSPI

#endif	// __TIMERBUILDER_H__
