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
	firstStartup(true),
	lastKnownMode(CEzspDongleMode::UNKNOWN),
	switchToFirmwareUpgradeOnInitTimeout(false),
	timer_factory(i_timer_factory),
	pUart(nullptr),
	uartIncomingDataHandler(),
	ash(static_cast<CAshCallback*>(this), timer_factory),
	blp(nullptr, timer_factory),
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
            this->lastKnownMode = CEzspDongleMode::EZSP_NCP;    /* We are now sure the dongle is communicating over ASH */
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
    else if( ASH_RESET_FAILED == info )
    {
        /* ASH reset failed */
        if (firstStartup)
        {
            /* If this is the startup sequence, we might be in bootloader prompt mode, not in ASH mode, so try to exit to EZSP/ASH mode from bootloader */
            if (this->switchToFirmwareUpgradeOnInitTimeout)
            {
                this->setMode(CEzspDongleMode::BOOTLOADER_FIRMWARE_UPGRADE);
            }
            else
            {
                this->setMode(CEzspDongleMode::BOOTLOADER_EXIT_TO_EZSP_NCP);
            }
            firstStartup = false;
        }
        else
        {
            clogE << "EZSP adapter is not responding\n";
            notifyObserversOfDongleState( DONGLE_NOT_RESPONDING );
        }
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
        if (this->lastKnownMode == CEzspDongleMode::EZSP_NCP || this->lastKnownMode == CEzspDongleMode::UNKNOWN)
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
            /* When switching to the bootloader, we are expecting a prompt (see class CBootloaderPrompt for more details) */
            blp.decode(li_data);
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
    if (this->lastKnownMode != CEzspDongleMode::EZSP_NCP && this->lastKnownMode != CEzspDongleMode::UNKNOWN)
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

void CEzspDongle::forceFirmwareUpgradeOnInitTimeout()
{
    this->switchToFirmwareUpgradeOnInitTimeout = true;
}

void CEzspDongle::setMode(CEzspDongleMode requestedMode)
{
    if (this->lastKnownMode != CEzspDongleMode::EZSP_NCP
        && (requestedMode == CEzspDongleMode::EZSP_NCP || requestedMode == CEzspDongleMode::BOOTLOADER_EXIT_TO_EZSP_NCP))
    {
        /* We are requested to get out of the booloader */
        this->lastKnownMode = requestedMode;
        this->blp.registerSerialWriteFunc([this](size_t& writtenCnt, const uint8_t* buf, size_t cnt) -> int {
            return this->pUart->write(writtenCnt, buf, cnt);
        });    /* Allow the blp object to write to the serial port via our own pUart attribute */
        this->blp.registerPromptDetectCallback([this]() {
            notifyObserversOfBootloaderPrompt();
            this->blp.selectModeRun(); /* As soon as we detect a bootloader prompt, we will request to run the application (EZSP NCP mode) */
            this->lastKnownMode = CEzspDongleMode::EZSP_NCP;   /* After launching the run command, we are in EZSP/ZSH mode */
            /* Restart the EZSP startup procedure here */
            this->open(this->pUart); /* FIXME: we don't need to run all the code from open(), just reset state variables, send the reset frame and arm again the ASH reset timeout */
        });
        this->blp.reset();    /* Reset the bootloader parser until we get a valid bootloader prompt */
        return;
    }
    if ((this->lastKnownMode == CEzspDongleMode::EZSP_NCP || this->lastKnownMode == CEzspDongleMode::UNKNOWN)
        && requestedMode == CEzspDongleMode::BOOTLOADER_FIRMWARE_UPGRADE)
    {
        clogD << "Attaching bootloader parser to serial port\n";
        /* We are requesting to switch from EZSP/ASH to bootloader parsing mode, and then perform a firmware upgrade */
        this->lastKnownMode = requestedMode;
        this->blp.registerSerialWriteFunc([this](size_t& writtenCnt, const uint8_t* buf, size_t cnt) -> int {
            return this->pUart->write(writtenCnt, buf, cnt);
        });    /* Allow the blp object to write to the serial port via our own pUart attribute */
        this->blp.registerPromptDetectCallback([this]() {
            notifyObserversOfBootloaderPrompt();
            /* Note: we provide selectModeUpgradeFw() with a callback that will be invoked when the firmware image transfer over serial link can start */
            /* This callback will only invoke our own notifyObserversOfFirmwareXModemXfrReady() method, that will in turn notify all observers that the firmware image transfer can start */
            this->blp.selectModeUpgradeFw([this]() { this->notifyObserversOfFirmwareXModemXfrReady(); });
            this->lastKnownMode = CEzspDongleMode::BOOTLOADER_FIRMWARE_UPGRADE;   /* After launching the upgrade command, we are in firmware upgrade mode (X-modem) */
        });
        this->blp.reset();    /* Reset the bootloader parser until we get a valid bootloader prompt */
        return;
    }
    clogE << "Adapter mode request combination in not implemented (last known="
          << static_cast<unsigned int>(this->lastKnownMode) << ", requested="
          << static_cast<unsigned int>(requestedMode) << ")\n";
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

void CEzspDongle::notifyObserversOfFirmwareXModemXfrReady() {
	for(auto observer : this->observers) {
		observer->handleFirmwareXModemXfr();
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
