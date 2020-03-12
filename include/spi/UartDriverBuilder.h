#ifndef __UARTDRIVDERBUILDER_H__
#define __UARTDRIVDERBUILDER_H__

#include <memory>
#include <functional>

#include <ezsp/export.h>

#ifdef USE_RARITAN
#include <pp/Selector.h>
#endif

#include <spi/IUartDriver.h>

namespace NSSPI {

class LIBEXPORT UartDriverBuilder {
public:
	/**
	 * @brief Default constructor
	 */
	UartDriverBuilder();

	/* Note: Raritan environment has an extra constructor allowing to pass the event loop selector when constructing this builder */
#ifdef USE_RARITAN
	/**
	 * @brief Constructor using a specified selector
	 *
	 * @param[in] selector The selector to use in the generated UART driver
	 */
	UartDriverBuilder(pp::Selector& selector);
#endif

	/**
	 * @brief Destructor
	 */
	~UartDriverBuilder() = default;

	/**
	 * @brief Create a UART driver
	 *
	 * @return The new UART driver created
	 */
	std::unique_ptr<IUartDriver> create() const;
private:
#ifdef USE_RARITAN
	pp::Selector& eventSelector;	/*!< The raritan event selector */
#endif
};

} // namespace NSSPI

#endif
