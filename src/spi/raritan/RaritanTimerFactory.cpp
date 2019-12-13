/**
 * @file RaritanTimerFactory.cpp
 *
 * @brief Concrete implementation of a ITimer factory returning RaritanTimer objects
 */

#include "RaritanTimerFactory.h"
#include "RaritanTimer.h"

RaritanTimerFactory::RaritanTimerFactory() {

}

RaritanTimerFactory::~RaritanTimerFactory() {

}

std::unique_ptr<ITimer> RaritanTimerFactory::create() const {
	return std::unique_ptr<ITimer>(new RaritanTimer());
}
