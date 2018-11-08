/**
 * 
 */

#include "ezsp-dongle.h"

#include "RaritanUartDriver.h"


CEzspDongle::CEzspDongle()
{
    uart = nullptr;
}

bool CEzspDongle::open(const std::string& serialPortName, unsigned int baudRate, EFlowControl flowControl)
{
    bool lo_success = true;
    std::vector<uint8_t> l_buffer;
    size_t l_size;

    if( nullptr != uart )
    {
        uart->close();
        delete uart;
    }

    // \todo instantiate the right uart driver according to system
    uart = new UartDriverRaritan();
    
    // open uart port /!\ assume open always work !
    open(serialPortName, baudRate);

    // reset ash ncp
    l_buffer = ash.resetNCPFrame();

    if( write(l_size, l_buffer.data(), l_buffer.size) < 0 )
    {
        // error
        lo_success = false;
    }
    else
    {
        if( l_size != l_buffer.size )
        {
            // error size mismatch
            lo_success = false;
        }
    }

    return lo_success;
}

