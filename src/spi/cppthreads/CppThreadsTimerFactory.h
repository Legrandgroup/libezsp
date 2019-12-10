/**
 * @file CppThreadsTimerFactory.h
 *
 * @brief Concrete implementation of a ITimer factory returning CppThreadsTimer objects
 */
#pragma once

#include "spi/ITimerFactory.h"
#include "CppThreadsTimer.h"

/**
 * @brief Factory class to generate CppThreadsTimer objects
 */
class CppThreadsTimerFactory : public ITimerFactory {
public:
	CppThreadsTimerFactory();
	~CppThreadsTimerFactory();
	std::unique_ptr<ITimer> create() const;
};
