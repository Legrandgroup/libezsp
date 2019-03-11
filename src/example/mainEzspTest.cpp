/**
 * @file mainEzspTest.cpp
 * 
 * @brief Sample code for driving a dongle in the Raritan framework or using libserialcpp
 */

#include <iostream>

#ifdef USE_SERIALCPP
#include "../spi/serial/SerialUartDriver.h"
#include "../spi/cppthreads/CppThreadsTimerFactory.h"
#include <string>
#else
#ifdef USE_RARITAN
#include "../spi/raritan/RaritanEventLoop.h"
#include "../spi/raritan/RaritanUartDriver.h"
#include "../spi/raritan/RaritanTimerFactory.h"
#else
#error Compiler directive USE_SERIALCPP or USE_RARITAN is required
#endif	// USE_RARITAN
#endif	// USE_SERIALCPP
#include "GenericLogger.h"
#include "CAppDemo.h"

using namespace std;

int main( void )
{
#ifdef USE_SERIALCPP
    SerialUartDriver uartDriver;
    CppThreadsTimerFactory timerFactory;
#endif
#ifdef USE_RARITAN
    RaritanEventLoop eventLoop;
    RaritanUartDriver uartDriver(eventLoop);
    RaritanTimerFactory timerFactory(eventLoop);

    RaritanLogger::getInstance().setLogLevel(LOG_LEVEL::INFO);	/* Only display logs for debug level info and higher (up to error) */
#endif

    clogI << "Starting ezsp test program (info)\n";

    uartDriver.open("/dev/ttyUSB0", 57600);

    CAppDemo app(&uartDriver, timerFactory);

#ifdef USE_SERIALCPP
    std::string line;
    std::getline(std::cin, line);
#endif
#ifdef USE_RARITAN
    eventLoop.run();
#endif

    return 0;
}
