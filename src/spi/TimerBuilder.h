/**
 * @file TimerBuilder.h
 *
 * @brief Concrete implementation of a ITimer factory
 */


#include "spi/ITimerBuilder.h"

/**
 * @brief Factory class to generate RaritanTimer objects
 */
class TimerBuilder : public ITimerBuilder {
public:
	/**
	 * @brief Constructor
	 *
	 * @param eventLoop The current process' Raritan mainloop
	 */
	TimerBuilder();

	/**
	 * @brief Destructor
	 */
	~TimerBuilder();

	/**
	 * @brief Create a new timer
	 *
	 * @return The new timer created
	 */
	std::unique_ptr<ITimer> create() const;
private:
};
