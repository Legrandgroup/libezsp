/**
 * @file RaritanEventLoop.cpp
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

	pp::Selector& getSelector();

private:
	pp::Selector& m_selector;
};
