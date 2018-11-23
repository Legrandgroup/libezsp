/**
 * @file mainEzscpTest
 * 
 * @brief Sample code for driving a dongle in the Raritan framework or using libserialcpp
 */

#include <iostream>

#ifdef USE_SERIALCPP
#include "../spi/serial/SerialUartDriver.h"
#include "../spi/serial/SerialTimerFactory.h"
#include <string>
#else
#include "../spi/raritan/RaritanEventLoop.h"
#include "../spi/raritan/RaritanUartDriver.h"
#include "../spi/raritan/RaritanTimerFactory.h"
#endif
#include "CAppDemo.h"

using namespace std;

int main( void )
{
#ifdef USE_SERIALCPP
    UartDriverSerial uartDriver;
    SerialTimerFactory timerFactory;
#else
    RaritanEventLoop eventLoop;
    UartDriverRaritan uartDriver(eventLoop);
    RaritanTimerFactory timerFactory(eventLoop);
#endif

    cout << "Starting ezsp test program !" << endl;

    uartDriver.open("/dev/ttyUSB0", 57600);

    CAppDemo app(&uartDriver, timerFactory);

#ifdef USE_SERIALCPP
    std::string line;
    std::getline(std::cin, line);
#else
    eventLoop.run();
#endif

    return 0;
}
