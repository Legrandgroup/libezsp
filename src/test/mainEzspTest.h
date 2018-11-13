/**
 * 
 * 
 */

#include "../domain/ezsp-dongle.h"
#include "../spi/IUartDriver.h"

typedef enum
{
    APP_NOT_INIT,
    APP_INIT_IN_PROGRESS,
    APP_READY
}EAppState;

class CAppDemo : public CDongleHandler
{
public:
    CAppDemo(IUartDriver *uartDriver);

    /**
     * Callback
     */
    void dongleState( EDongleState i_state );
    void ashRxMessage( std::vector<uint8_t> i_message );
    void ezspHandler( EEzspCmd i_cmd, std::vector<uint8_t> i_message );

private:
    CEzspDongle dongle;
    EAppState app_state;

    void dongleInit();
    void stackInit();
};

