/**
 * @file ezsp-dongle.cpp
 */
#include <sstream>
#include <iomanip>

#include "ezsp-dongle.h"
#include "spi/ILogger.h"
#include "spi/GenericAsyncDataInputObservable.h"

using NSEZSP::CEzspDongle;

CEzspDongle::CEzspDongle( NSSPI::TimerBuilder &i_timer_factory, CEzspDongleObserver* ip_observer ) :
	bootloaderMode(false),
	timer_factory(i_timer_factory),
	pUart(nullptr),
	uartIncomingDataHandler(),
	ash(static_cast<CAshCallback*>(this), timer_factory),
	sendingMsgQueue(),
	wait_rsp(false),
	observers()
{
    if( nullptr != ip_observer )
    {
        registerObserver(ip_observer);
    }
}

bool CEzspDongle::open(NSSPI::IUartDriver *ipUart)
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

        // Send a ASH reset to the NCP
        l_buffer = ash.resetNCPFrame();

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
        if( ash.isConnected() )
        {
            notifyObserversOfDongleState( DONGLE_READY );
        }
        else
        {
            notifyObserversOfDongleState( DONGLE_REMOVE );
        }
    }
    else if ( ASH_NACK == info )
    {
        clogW << "Caught an ASH NACK from NCP... resending\n";
        wait_rsp = false;
        sendNextMsg();
    }
    else
    {
        clogW << "Caught an unknown ASH\n";
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
            lo_msg = ash.decode(li_data);

            // send incomming mesage to application
            if( !lo_msg.empty() )
            {
                size_t l_size;

                //clogD << "CEzspDongle::handleInputData ash message decoded" << std::endl;

                // send ack
                std::vector<uint8_t> l_msg = ash.AckFrame();
                pUart->write(l_size, l_msg.data(), l_msg.size());

                // call handler

                // ezsp
                // extract ezsp command
                EEzspCmd l_cmd = static_cast<EEzspCmd>(lo_msg.at(2));
                // remove the EZSP header
                lo_msg.erase(lo_msg.begin(),lo_msg.begin()+3);  /* FIXME: make sure buffer is more than 2 bytes large */
                // remove the EZSP CRC16
                lo_msg.erase(lo_msg.end()-2,lo_msg.end());  /* FIXME: make sure buffer is more than 2 bytes large */

                this->handleResponse(l_cmd);
                // notify observers
                notifyObserversOfEzspRxMessage( l_cmd, lo_msg );
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
#if 1   /* Just dump the byte stream */
            std::stringstream msg;
            msg << "Read buffer from serial port:";
            for (size_t loop=0; loop<li_data.size(); loop++) {
                msg << " " << std::hex << std::setw(2) << std::setfill('0') <<
                    +(static_cast<const unsigned char>(li_data[loop]));
            }
            msg << "\n";
            clogE << msg.str();
#endif
            li_data.clear();
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

        li_data.push_back(static_cast<uint8_t>(l_msg.i_cmd));
        li_data.insert(li_data.end(), l_msg.payload.begin(), l_msg.payload.end() ); /* Append payload at the end of li_data */

        //-- clogD << "CEzspDongle::sendCommand ash.DataFrame" << std::endl;
        l_enc_data = ash.DataFrame(li_data);
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

void CEzspDongle::setBootloaderMode(bool dongleInBootloaderMode)
{
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

void CEzspDongle::handleDongleState( EDongleState i_state )
{
	// do nothing
}

void CEzspDongle::handleResponse( EEzspCmd i_cmd )
{
	// response to a sending command
	if( !sendingMsgQueue.empty() )
	{
		if (!wait_rsp)
		{
			/* If wait_rsp is false, we are not expecting a response to a previous command.
			   But sendingMsgQueue should always contain (at front) the last command sent without reply, so when sendingMsgQueue is not empty,
			   wait_rsp should be true
			*/
			clogE << "Received a message with a non-empty queue while no response was expected\n";
		}
		sMsg l_msgQ = sendingMsgQueue.front();
		if( l_msgQ.i_cmd == i_cmd ) /* Make sure that the EZSP message is a response to the last command we sent */
		{
			// remove waiting message and send next
			sendingMsgQueue.pop();
			wait_rsp = false;
			sendNextMsg();
		}    // response to a sending command
		else
		{
			clogE << "Asynchronous received EZSP message\n";
		}
	}
}
