/**
 * @file TimerBuilder.cpp
 *
 * @brief Concrete implementation of a ITimer factory returning RaritanTimer objects
 */

#include "spi/TimerBuilder.h"
#ifdef USE_RARITAN
#include "spi/raritan/RaritanTimer.h"
#endif
#ifdef USE_CPPTHREADS
#include "spi/cppthreads/CppThreadsTimer.h"
#endif

TimerBuilder::TimerBuilder() {

}

TimerBuilder::~TimerBuilder() {

}

std::unique_ptr<ITimer> TimerBuilder::create() const {
#ifdef USE_RARITAN
	return std::unique_ptr<ITimer>(new RaritanTimer());
#endif
#ifdef USE_CPPTHREADS
	return std::unique_ptr<ITimer>(new CppThreadsTimer());
#endif
}
