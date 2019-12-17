/**
 * @file TimerFactory.cpp
 *
 * @brief Concrete implementation of a ITimer factory returning RaritanTimer objects
 */

#include "spi/TimerFactory.h"
#ifdef USE_RARITAN
#include "spi/raritan/RaritanTimer.h"
#endif
#ifdef USE_CPPTHREADS
#include "spi/cppthreads/CppThreadsTimer.h"
#endif

TimerFactory::TimerFactory() {

}

TimerFactory::~TimerFactory() {

}

std::unique_ptr<ITimer> TimerFactory::create() const {
#ifdef USE_RARITAN
	return std::unique_ptr<ITimer>(new RaritanTimer());
#endif
#ifdef USE_CPPTHREADS
	return std::unique_ptr<ITimer>(new CppThreadsTimer());
#endif
}
