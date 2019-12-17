/**
 * @file TimerFactory.h
 *
 * @brief Concrete implementation of a ITimer factory
 */


#include "spi/ITimerFactory.h"

/**
 * @brief Factory class to generate RaritanTimer objects
 */
class TimerFactory : public ITimerFactory {
public:
	/**
	 * @brief Constructor
	 *
	 * @param eventLoop The current process' Raritan mainloop
	 */
	TimerFactory();

	/**
	 * @brief Destructor
	 */
	~TimerFactory();

	/**
	 * @brief Create a new timer
	 *
	 * @return The new timer created
	 */
	std::unique_ptr<ITimer> create() const;
private:
};
