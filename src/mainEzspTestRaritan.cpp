/**
 * @file mainEzscpTestRaritan
 * 
 * @brief Sample code for driving a dongle in the Raritan framework
 */

#include <iostream>

#include "spi/raritan/RaritanEventLoop.h"
#include "spi/raritan/RaritanUartDriver.h"
#include "spi/raritan/RaritanTimerFactory.h"
#include "test/mainEzspTest.h"

using namespace std;

int main( void )
{
    CAppDemo *app;
    RaritanEventLoop eventLoop;
    UartDriverRaritan uartDriver(eventLoop);
    RaritanTimerFactory timerFactory(eventLoop);

    cout << "Starting ezsp test program !" << endl;

    uartDriver.open("/dev/ttyUSB0", 57600);

    app = new CAppDemo(&uartDriver, timerFactory);

    eventLoop.run();

    delete app;

    return 0;
}
