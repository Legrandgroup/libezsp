#pragma once

#ifdef USE_SERIALCPP
#include "spi/console/ConsoleLogger.h"
#else
#ifdef USE_RARITAN
#include "spi/raritan/RaritanLogger.h"
#else
#error Compiler directive USE_SERIALCPP or USE_RARITAN is required
#endif	// USE_RARITAN
#endif	// USE_SERIALCPP
