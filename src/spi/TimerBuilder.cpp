/**
 * @file TimerBuilder.cpp
 *
 * @brief Builder returning a concrete implementation of ITimer objects (matching with the architecture)
 */

#include "spi/TimerBuilder.h"

#undef __TIMER_SPI_FOUND__ //NOSONAR
#ifdef USE_RARITAN
#define __TIMER_SPI_FOUND__
#include "spi/raritan/RaritanTimer.h"
namespace NSSPI {
typedef RaritanTimer Timer;
}
#endif
#ifdef USE_CPPTHREADS
# ifdef __TIMER_SPI_FOUND__
#  error Duplicate timer SPI in use
# endif
#define __TIMER_SPI_FOUND__
#include "spi/cppthreads/CppThreadsTimer.h"
namespace NSSPI {
typedef CppThreadsTimer Timer;
}
#endif
#ifndef __TIMER_SPI_FOUND__
# error At least one timer SPI should be selected
#endif
#undef __TIMER_SPI_FOUND__ //NOSONAR

using NSSPI::TimerBuilder;
using NSSPI::ITimer;

#ifdef USE_RARITAN
TimerBuilder::TimerBuilder(pp::Selector& selector) : eventSelector(selector) {
}

TimerBuilder::TimerBuilder() : TimerBuilder(*pp::SelectorSingleton::getInstance()) {
}
#else	// USE_RARITAN
TimerBuilder::TimerBuilder() = default;
#endif	// USE_RARITAN

std::unique_ptr<ITimer> TimerBuilder::create() const {
#ifdef USE_RARITAN
	/* TODO: When using a C++14 compliant compiler, the line below should be replaced with:
	 * return std::make_unique<NSSPI::RaritanTimer>(this->eventSelector);
	 */
	return std::unique_ptr<ITimer>(new NSSPI::RaritanTimer(this->eventSelector));
#else // USE_RARITAN
	/* TODO: When using a C++14 compliant compiler, the line below should be replaced with:
	 * return std::make_unique<NSSPI::Timer>();
	 * Note: disabling code anaylis below because SonarCloud insists on blacklisting any dynamic allocation
	 * but we really require it is a factory or builder, and using it is mitigated by the unique_ptr container
	 */
	return std::unique_ptr<ITimer>(new NSSPI::Timer()); //NOSONAR
#endif	// #ifdef USE_CPPTHREADS
}

/**
 * This method is a friend of NSSPI::TimerBuilder class
**/
void swap(NSSPI::TimerBuilder& first, NSSPI::TimerBuilder& second) { /* nothrow */
	using std::swap;	// Enable ADL

#ifdef USE_RARITAN
	/* We whould use swap() here to be exception safe but pp::Selector does not implement the swap() method */
	/* Because we are dealing with references, there is no risk of exception during the following 3 lines, which is what we must ensure in this swap() method */
	pp::Selector& tempFirst = temp.eventSelector;
	first.eventSelector = second.eventSelector;
	second.eventSelector = temp;
#endif
	/* Once we have swapped the members of the two instances... the two instances have actually been swapped */
}
