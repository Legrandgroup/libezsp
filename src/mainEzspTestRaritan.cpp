/**
 * 
 * 
 */

#include <iostream>

#include "src/spi/raritan/RaritanEventLoop.h"
#include "src/spi/raritan/RaritanUartDriver.h"
#include "src/test/mainEzspTest.h"

using namespace std;

int main( void )
{
    CAppDemo *app;
    RaritanEventLoop eventLoop;
    UartDriverRaritan uartDriver(eventLoop);

    cout << "Starting ezsp test program !" << endl;

    uartDriver.open("/dev/ttyUSB0", 57600);

    app = new CAppDemo(&uartDriver);

    eventLoop->run();

    delete app;

    return 0;
}