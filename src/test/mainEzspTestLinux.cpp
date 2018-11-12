/**
 * 
 * 
 */

#include <iostream>

#include "mainEzspTest.h"
// #include "../spi/serial/SerialUartDriver.h"

using namespace std;

int main( void )
{
    CAppDemo *app;
	// UartDriverSerial uartDriver(uartIncomingDataHandler);


    std::cout << "Starting ezsp test program !" << std::endl;

	// uartDriver.open("/dev/ttyUSB0", 57600);

    app = new CAppDemo( nullptr /*uartDriver*/);

    while(1)
        ;
    

    delete app;

    return 0;
}