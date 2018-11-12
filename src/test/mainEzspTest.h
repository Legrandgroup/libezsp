/**
 * 
 * 
 */

#include "../domain/ezsp-dongle.h"
#include "../spi/IUartDriver.h"

class CAppDemo : public CEzspHandler
{
public:
    CAppDemo(IUartDriver *uartDriver);

    /**
     * Callback
     */
    void ashRxMessage( std::vector<uint8_t> i_message );
    void ezspHandler( EEzspCmd i_cmd, std::vector<uint8_t> i_message );

private:
    CEzspDongle dongle;

    void stackInit();
};

