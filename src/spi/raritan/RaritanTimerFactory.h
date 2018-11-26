/**
 * @file RaritanTimerFactory.h
 *
 * @brief Concrete implementation of a ITimer factory returning RaritanTimer objects
 */

#pragma once

#include "../ITimerFactory.h"
#include "RaritanEventLoop.h"

/**
 * @brief Factory class to generate RaritanTimer objects
 */
class RaritanTimerFactory : public ITimerFactory {
public:
	/**
	 * @brief Constructor
	 *
	 * @param eventLoop The current process' Raritan mainloop
	 */
	RaritanTimerFactory(RaritanEventLoop& eventLoop);

	/**
	 * @brief Destructor
	 */
	~RaritanTimerFactory();

	/**
	 * @brief Create a new timer
	 *
	 * @return The new timer created
	 */
	std::unique_ptr<ITimer> create() const;
private:
	RaritanEventLoop& m_eventLoop;	/*!< The Raritan event loop */
};
