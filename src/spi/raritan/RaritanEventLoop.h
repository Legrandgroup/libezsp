/**
 * @file RaritanEventLoop.h
 */

#pragma once

#include <pp/Selector.h>

class RaritanEventLoop {
public:
	RaritanEventLoop();

	/**
	 * @brief Run the main loop
	 */
	void run();

	/**
	 * @brief Get the main loop selector
	 */
	pp::Selector& getSelector();

private:
	pp::Selector& m_selector;	/*!< The main loop selector */
};
