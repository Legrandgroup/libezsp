/**
 * 
 * 
 */

#include <iostream>

#include "mainEzspTest.h"
#include "../spi/serial/SerialUartDriver.h"
#include "../spi/serial/SerialTimerFactory.h"

using namespace std;

int main( void )
{
    CAppDemo *app;
	UartDriverSerial uartDriver;
    SerialTimerFactory timerFactory;



    std::cout << "Starting ezsp test program !" << std::endl;

	uartDriver.open("/dev/ttyUSB0", 57600);

    app = new CAppDemo( &uartDriver, timerFactory );

    while(1)
        ;
    

    delete app;

    return 0;
}