/**
 * @file mainEzspTest.cpp
 * 
 * @brief Sample code for driving a dongle in the Raritan framework or using libserialcpp
 */

#include <iostream>

#ifdef USE_SERIALCPP
#include "../spi/serial/SerialUartDriver.h"
#include "../spi/serial/SerialTimerFactory.h"
//#include "../spi/serial/ConsoleLogger.h"
#include <string>
#else
#ifdef USE_RARITAN
#include "../spi/raritan/RaritanEventLoop.h"
#include "../spi/raritan/RaritanUartDriver.h"
#include "../spi/raritan/RaritanTimerFactory.h"
#include "../spi/raritan/RaritanLogger.h"
#else
#error Compiler directive USE_SERIALCPP or USE_RARITAN is required
#endif	// USE_RARITAN
#endif	// USE_SERIALCPP
#include "CAppDemo.h"

using namespace std;

int main( void )
{
#ifdef USE_SERIALCPP
    UartDriverSerial uartDriver;
    SerialTimerFactory timerFactory;
//    ConsoleLogger logger; // TODO
#endif
#ifdef USE_RARITAN
    RaritanEventLoop eventLoop;
    UartDriverRaritan uartDriver(eventLoop);
    RaritanTimerFactory timerFactory(eventLoop);
    RaritanLogger logger;
#endif

    logger.log(RaritanLogger::LOG_LEVEL::INFO, "Starting ezsp test program\n");

    uartDriver.open("/dev/ttyUSB0", 57600);

    CAppDemo app(&uartDriver, timerFactory, logger);

#ifdef USE_SERIALCPP
    std::string line;
    std::getline(std::cin, line);
#endif
#ifdef USE_RARITAN
    eventLoop.run();
#endif

    return 0;
}
