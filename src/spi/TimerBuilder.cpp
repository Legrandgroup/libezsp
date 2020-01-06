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

using namespace NSSPI;

std::unique_ptr<ITimer> TimerBuilder::create() const {
#ifdef USE_RARITAN
	static RaritanTimer gTimer;
#endif
#ifdef USE_CPPTHREADS
	static CppThreadsTimer gTimer;
#endif
	return std::unique_ptr<ITimer>(&gTimer);
}

