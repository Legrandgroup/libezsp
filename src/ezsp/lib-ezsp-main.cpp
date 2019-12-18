/**
 * @file lib-ezsp-main.cpp
 */

#include "ezsp/lib-ezsp-main.h"
#include "spi/ILogger.h"


CLibEzspMain::CLibEzspMain(IUartDriver *uartDriver,
        TimerBuilder &i_timer_factory) :
    lib_state(CLibEzspState::NO_INIT),
    obsStateCallback(nullptr),
    timer(i_timer_factory.create()),
    dongle(i_timer_factory, this),
    zb_messaging(dongle, i_timer_factory),
    zb_nwk(dongle, zb_messaging),
    gp_sink(dongle, zb_messaging)
{
    setState(CLibEzspState::INIT_FAILED);

    // uart
    if( dongle.open(uartDriver) ) {
        clogI << "CLibEzspMain open success !" << std::endl;
        dongle.registerObserver(this);
        gp_sink.registerObserver(this);
        setState(CLibEzspState::INIT_IN_PROGRESS);
    }
}


void CLibEzspMain::setState( CLibEzspState i_new_state )
{ 
    lib_state=i_new_state; 
    /* \todo inform observe of state changed */
    if( nullptr != obsStateCallback )
    {
        obsStateCallback(i_new_state);
    }
}

/**
 * Oberver handlers
 */
void CLibEzspMain::handleDongleState( EDongleState i_state )
{

}

void CLibEzspMain::handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive )
{

}

void CLibEzspMain::handleRxGpFrame( CGpFrame &i_gpf )
{

}

void CLibEzspMain::handleRxGpdId( uint32_t &i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status )
{

}
