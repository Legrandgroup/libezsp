/**
 * 
 */

#include "ezsp-dongle.h"


CEzspDongle::CEzspDongle( CEzspHandler *ipCb )
{
    pUart = nullptr;
    pCb = ipCb;
    ash = new CAsh((CAshCallback*)this, nullptr);
}

CEzspDongle::~CEzspDongle()
{
    pUart = nullptr;
    delete ash;
}

bool CEzspDongle::open(IUartDriver *ipUart)
{
    bool lo_success = true;
    std::vector<uint8_t> l_buffer;
    size_t l_size;

    if( nullptr == ipUart )
    {
        lo_success = false;
    }
    else
    {
        pUart = ipUart;

        // reset ash ncp
        l_buffer = ash->resetNCPFrame();

        if( pUart->write(l_size, l_buffer.data(), l_buffer.size()) < 0 )
        {
            // error
            lo_success = false;
            pUart = nullptr;
        }
        else
        {
            if( l_size != l_buffer.size() )
            {
                // error size mismatch
                lo_success = false;
                pUart = nullptr;
            }
        }
    }

    return lo_success;
}

void CEzspDongle::handleInputData(const unsigned char* dataIn, const size_t dataLen)
 {
     /*
    std::stringstream bufDump;

    for (size_t i =0; i<dataLen; i++) {
        bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(dataIn[i]) << " ";
    }
    cout << name << ": Received buffer " << bufDump.str() << endl;
    */
}