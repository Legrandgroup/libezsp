/**
 * 
 * 
 */

#include <iostream>

#include "spi/raritan/RaritanEventLoop.h"
#include "spi/raritan/RaritanUartDriver.h"
#include "test/mainEzspTest.h"

using namespace std;

int main( void )
{
    CAppDemo *app;
    RaritanEventLoop eventLoop;
    UartDriverRaritan uartDriver(eventLoop);

    cout << "Starting ezsp test program !" << endl;

    uartDriver.open("/dev/ttyUSB0", 57600);

    app = new CAppDemo(&uartDriver);

    eventLoop.run();

    delete app;

    return 0;
}