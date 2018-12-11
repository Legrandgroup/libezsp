/**
 * @file RaritanEventLoop.h
 *
 * @brief Allows manipulating the main loop in the Raritan framwork
 */

#pragma once

#include <pp/Selector.h>

/**
 * @brief Class to manipulate the main loop in the Raritan framwork
 */
class RaritanEventLoop {
public:
	/**
	 * @brief Default constructor
	 */
	RaritanEventLoop();
	RaritanEventLoop(pp::Selector &selector);
	/**
	 * @brief Run the main loop
	 */
	void run();

	/**
	 * @brief Get the main loop selector
	 *
	 * @return The main loop selector as a reference
	 */
	pp::Selector& getSelector();

private:
	pp::Selector& m_selector;	/*!< The main loop selector */
};
