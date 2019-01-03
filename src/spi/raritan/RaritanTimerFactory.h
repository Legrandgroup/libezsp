/**
 * @file RaritanTimerFactory.h
 *
 * @brief Concrete implementation of a ITimer factory returning RaritanTimer objects
 */

#pragma once

#include "../ITimerFactory.h"
#include "RaritanEventLoop.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

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

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
