/**
 * @file TimerBuilder.cpp
 *
 * @brief Builder returning a concrete implementation of ITimer objects (matching with the architecture)
 */

#include "spi/TimerBuilder.h"
#ifdef USE_RARITAN
#include "spi/raritan/RaritanTimer.h"
#endif
#ifdef USE_CPPTHREADS
#include "spi/cppthreads/CppThreadsTimer.h"
#endif

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

