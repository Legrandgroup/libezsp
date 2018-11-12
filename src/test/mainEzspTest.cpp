


#include "mainEzspTest.h"


CAppDemo::CAppDemo(IUartDriver *uartDriver) : dongle(this) { 
    // uart
    dongle.open(uartDriver);
}
