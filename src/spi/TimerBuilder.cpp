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

using NSSPI::TimerBuilder;
using NSSPI::ITimer;

#ifdef USE_RARITAN
	typedef class NSSPI::RaritanTimer Timer;
#endif
#ifdef USE_CPPTHREADS
	typedef class NSSPI::CppThreadsTimer Timer;
#endif

std::unique_ptr<ITimer> TimerBuilder::create() const {
	return std::unique_ptr<ITimer>(new Timer());
}

