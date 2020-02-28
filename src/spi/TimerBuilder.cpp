/**
 * @file TimerBuilder.cpp
 *
 * @brief Builder returning a concrete implementation of ITimer objects (matching with the architecture)
 */

#include "spi/TimerBuilder.h"
#undef __TIMER_BUILDER_SPI_FOUND__
#ifdef USE_RARITAN
#define __TIMER_BUILDER_SPI_FOUND__
#include "spi/raritan/RaritanTimer.h"
#endif
#ifdef USE_CPPTHREADS
# ifdef __TIMER_BUILDER_SPI_FOUND__
#  error Duplicate timer builder SPI in use
# endif
#define __TIMER_BUILDER_SPI_FOUND__
#include "spi/cppthreads/CppThreadsTimer.h"
#endif
#ifndef __TIMER_BUILDER_SPI_FOUND__
# error At least one timer builder SPI should be selected
#endif
#undef __TIMER_BUILDER_SPI_FOUND__

using NSSPI::TimerBuilder;
using NSSPI::ITimer;

#ifdef USE_RARITAN
TimerBuilder::TimerBuilder(const pp::Selector& selector) : eventSelector(selector) {
}

TimerBuilder::TimerBuilder() : TimerBuilder(*pp::SelectorSingleton::getInstance()) {
}
#endif	// USE_RARITAN

#ifdef USE_CPPTHREADS
TimerBuilder::TimerBuilder() {
}
#endif

std::unique_ptr<ITimer> TimerBuilder::create() const {
#ifdef USE_RARITAN
	return std::unique_ptr<ITimer>(new NSSPI::RaritanTimer(selector));
#endif
#ifdef USE_CPPTHREADS
	return std::unique_ptr<ITimer>(new NSSPI::CppThreadsTimer());
#endif
}

