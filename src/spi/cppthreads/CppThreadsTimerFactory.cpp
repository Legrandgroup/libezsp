#include "CppThreadsTimerFactory.h"

CppThreadsTimerFactory::CppThreadsTimerFactory() { }

CppThreadsTimerFactory::~CppThreadsTimerFactory() { }

std::unique_ptr<ITimer> CppThreadsTimerFactory::create() const {
	return std::unique_ptr<ITimer>(new CppThreadsTimer());
}