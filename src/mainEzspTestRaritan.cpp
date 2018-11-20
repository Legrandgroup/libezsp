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
#include "domain/byte-manip.h"

using namespace std;

int main( void )
{
    CAppDemo *app;
    RaritanEventLoop eventLoop;
    UartDriverRaritan uartDriver(eventLoop);
    RaritanTimerFactory timerFactory(eventLoop);

    uint16_t test = dble_u8_to_u16(0x56, 0x89);
    cout << "Hi(0x56)+Low(0x89)=" << test << "\n";
    cout << "Hi=" << u16_get_hi_u8(test) << ", Low=" << u16_get_lo_u8(test) << "\n";
    cout << "Starting ezsp test program !" << endl;

    uartDriver.open("/dev/ttyUSB0", 57600);

    app = new CAppDemo(&uartDriver, timerFactory);

    eventLoop.run();

    delete app;

    return 0;
}
