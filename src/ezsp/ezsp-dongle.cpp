/**
 * @file ezsp-dongle.cpp
 */

#include "ezsp-dongle.h"
#include "spi/ILogger.h"
#include <sstream>
#include <iomanip>

CEzspDongle::CEzspDongle( TimerBuilder &i_timer_factory, CEzspDongleObserver* ip_observer ) :
	firstStartup(true),
	bootloaderMode(false),
	timer_factory(i_timer_factory),
	pUart(nullptr),
	ash(new CAsh(static_cast<CAshCallback*>(this), timer_factory)),
	blp(new CBootloaderPrompt(nullptr, timer_factory)),
	uartIncomingDataHandler(),
	sendingMsgQueue(),
	wait_rsp(false),
	observers()
{
    if( nullptr != ip_observer )
    {
        registerObserver(ip_observer);
    }
}

CEzspDongle::~CEzspDongle()
{
    pUart = nullptr;
    delete ash;
    delete blp;
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
            else
            {
                clogD << "CEzspDongle::open register uart !" << std::endl;
                uartIncomingDataHandler.registerObserver(this);
                pUart->setIncomingDataHandler(&uartIncomingDataHandler);
            }
        }
    }

    return lo_success;
}

void CEzspDongle::ashCbInfo( EAshInfo info ) 
{ 
    clogD <<  "ashCbInfo : " << CAsh::EAshInfoToString(info) << std::endl;

    if( ASH_STATE_CHANGE == info )
    {
        // inform upper layer that dongle is ready !
        if( ash->isConnected() )
        {
            notifyObserversOfDongleState( DONGLE_READY );
        }
        else
        {
            notifyObserversOfDongleState( DONGLE_REMOVE );
        }
    }
    else if( ASH_RESET_FAILED == info )
    {
        /* ASH reset failed */
        if (firstStartup)
        {
            /* If this is the startup sequence, we might be in bootloader prompt mode, not in ASH mode, so give it a try */
            setBootloaderMode(true);
            firstStartup = false;
        }
        else
        {
            notifyObserversOfDongleState( DONGLE_NOT_RESPONDING );
        }
    }
}

void CEzspDongle::handleInputData(const unsigned char* dataIn, const size_t dataLen)
 {
    std::vector<uint8_t> li_data;
    std::vector<uint8_t> lo_msg;

    li_data.clear();
    for( size_t loop=0; loop< dataLen; loop++ )
    {
        li_data.push_back(dataIn[loop]);
    }

    while( !li_data.empty())
    {
        if(! bootloaderMode )
        {
            lo_msg = ash->decode(li_data);

            // send incomming mesage to application
            if( !lo_msg.empty() )
            {
                size_t l_size;

                //clogD << "CEzspDongle::handleInputData ash message decoded" << std::endl;

                // send ack
                std::vector<uint8_t> l_msg = ash->AckFrame();
                pUart->write(l_size, l_msg.data(), l_msg.size());

                // call handler

                // ezsp
                // extract ezsp command
                EEzspCmd l_cmd = static_cast<EEzspCmd>(lo_msg.at(2));
                // keep only payload
                lo_msg.erase(lo_msg.begin(),lo_msg.begin()+3);

                // notify observers
                notifyObserversOfEzspRxMessage( l_cmd, lo_msg );


                // response to a sending command
                if( !sendingMsgQueue.empty() )
                {
                    sMsg l_msgQ = sendingMsgQueue.front();
                    if( l_msgQ.i_cmd == l_cmd ) // Bug
                    {
                        // remove waiting message and send next
                        sendingMsgQueue.pop();
                        wait_rsp = false;
                        sendNextMsg();
                    }
                }
            }
        }
        else
        {
            /* No ash decoding in bootloader mode */
            /* At start of bootloader, we are expecting a prompt:
Gecko Bootloader v1.6.0
1. upload gbl
2. run
3. ebl info
BL >
begin upload
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
*/
            blp->decode(li_data);
        }
    }    
}

void CEzspDongle::sendCommand(EEzspCmd i_cmd, std::vector<uint8_t> i_cmd_payload )
{
    sMsg l_msg;

    l_msg.i_cmd = i_cmd;
    l_msg.payload = i_cmd_payload;
    
    sendingMsgQueue.push(l_msg);

    sendNextMsg();
}


/**
 * 
 * PRIVATE
 * 
 */

void CEzspDongle::sendNextMsg( void )
{
    if (bootloaderMode)
    {
        clogW << "Refusing to send EZSP messages in bootloader mode\n";
        return; /* No EZSP message can be sent in bootloader mode */
    }
    if( (!wait_rsp) && (!sendingMsgQueue.empty()) )
    {
        sMsg l_msg = sendingMsgQueue.front();

        // encode command using ash and write to uart
        std::vector<uint8_t> li_data;
        std::vector<uint8_t> l_enc_data;
        size_t l_size;

        li_data.clear();
        li_data.push_back(static_cast<uint8_t>(l_msg.i_cmd));
        for( size_t loop=0; loop< l_msg.payload.size(); loop++ )
        {
            li_data.push_back(l_msg.payload.at(loop));
        }

        //-- clogD << "CEzspDongle::sendCommand ash->DataFrame" << std::endl;
        l_enc_data = ash->DataFrame(li_data);
        if( nullptr != pUart )
        {
            //-- clogD << "CEzspDongle::sendCommand pUart->write" << std::endl;
            pUart->write(l_size, l_enc_data.data(), l_enc_data.size());

            wait_rsp = true;
        }
    }
}


/**
 * Managing Observer of this class
 */
bool CEzspDongle::registerObserver(CEzspDongleObserver* observer)
{
    return this->observers.emplace(observer).second;
}

bool CEzspDongle::unregisterObserver(CEzspDongleObserver* observer)
{
    return static_cast<bool>(this->observers.erase(observer));
}

void CEzspDongle::setBootloaderMode(bool dongleInBootloaderMode)    /* FIXME: rename this to selectBootloaderOption and provide an enum with what we want (run, upgrade) */
{
    if (!bootloaderMode && dongleInBootloaderMode)
    {
        /* We are requesting to switch to bootloader parsing mode, while we were previously in EZSP parsing mode */
        this->blp->registerSerialWriteFunc([this](size_t& writtenCnt, const void* buf, size_t cnt) -> int {
            return this->pUart->write(writtenCnt, buf, cnt);
        });    /* Allow the blp object to write to the serial port via our own pUart attribute */
        this->blp->registerPromptDetectCallback([this]() -> std::vector<uint8_t> {
            notifyObserversOfBootloaderPrompt();
            this->blp->selectModeRun();
            this->bootloaderMode = false;   /* After launching the run command, we are not in bootloader prompt anymore */
            return std::vector<uint8_t>();  /* FIXME: we can't return a unique value here because all observers may want to say something... */
        });
        this->blp->reset();    /* Reset the bootloader parser until we get a valid bootloader prompt */
    }
    this->bootloaderMode = dongleInBootloaderMode;
}

void CEzspDongle::notifyObserversOfDongleState( EDongleState i_state ) {
	for(auto observer : this->observers) {
		observer->handleDongleState(i_state);
	}
}

void CEzspDongle::notifyObserversOfEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_message ) {
	for(auto observer : this->observers) {
		observer->handleEzspRxMessage(i_cmd, i_message);
	}
}

void CEzspDongle::notifyObserversOfBootloaderPrompt() {
	for(auto observer : this->observers) {
		observer->handleBootloaderPrompt();
	}
}