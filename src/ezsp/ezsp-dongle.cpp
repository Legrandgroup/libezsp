/**
 * @file ezsp-dongle.cpp
 */
#include <sstream>
#include <iomanip>

#include "ezsp-dongle.h"
#include "spi/ILogger.h"

DEFINE_ENUM(Mode, EZSP_DONGLE_MODE_LIST, NSEZSP::CEzspDongle);

using NSEZSP::CEzspDongle;

CEzspDongle::CEzspDongle(const NSSPI::TimerBuilder& i_timer_builder, CEzspDongleObserver* ip_observer) :
	firstStartup(true),
	version(),
	lastKnownMode(CEzspDongle::Mode::UNKNOWN),
	switchToFirmwareUpgradeOnInitTimeout(false),
	timerBuilder(i_timer_builder),
	uartHandle(nullptr),
	uartIncomingDataHandler(),
	ash(static_cast<CAshCallback*>(this), timerBuilder),
	blp(timerBuilder),
	sendingMsgQueue(),
	wait_rsp(false),
	observers() {
	if (ip_observer) {
		registerObserver(ip_observer);
	}
}

void CEzspDongle::setUart(NSSPI::IUartDriverHandle uartHandle) {
	this->uartHandle = uartHandle;
}

bool CEzspDongle::reset() {
	NSSPI::ByteBuffer l_buffer;
	size_t l_size;

	if (this->uartHandle == nullptr) {
		clogE << "No UART usable driver when invoking reset()\n";
		return false;
	}
	else {
		// Send a ASH reset to the NCP
		l_buffer = ash.resetNCPFrame();

		if (this->uartHandle->write(l_size, l_buffer.data(), l_buffer.size()) < 0 ) {
			clogE << "Failed sending reset frame to serial port\n";
			return false;
		}
		else {
			if (l_buffer.size() != l_size) {
				clogE << "Reset frame not fully written to serial port\n";
				return false;
			}
			else {
				clogD << "CEzspDongle UART reset\n";
				this->uartIncomingDataHandler.registerObserver(this);
				this->uartHandle->setIncomingDataHandler(&uartIncomingDataHandler);
			}
		}
	}

	return true;
}

void CEzspDongle::setFetchedXncpData(uint16_t xncpManufacturerId, uint16_t xncpVersionNumber) {
	this->version.setXncpData(xncpManufacturerId, xncpVersionNumber);
	this->notifyObserversOfDongleState(DONGLE_VERSION_RETRIEVED);   /* Notify observers that we now know the EZSP adapter's version */
}

void CEzspDongle::setFetchedEzspVersionData(uint16_t ezspStackVersion) {
	this->version.setEzspVersionInfo(ezspStackVersion);
	this->notifyObserversOfDongleState(DONGLE_VERSION_RETRIEVED);   /* Notify observers that we now know the EZSP adapter's version */
}

void CEzspDongle::setFetchedEzspVersionData(uint16_t ezspStackVersion, uint8_t ezspProtocolVersion, uint8_t ezspStackType) {
	this->version.setEzspVersionInfo(ezspStackVersion, ezspProtocolVersion, ezspStackType);
	this->notifyObserversOfDongleState(DONGLE_VERSION_RETRIEVED);   /* Notify observers that we now know the EZSP adapter's version */
}

NSEZSP::EzspAdapterVersion CEzspDongle::getVersion() const {
	return this->version;
}

void CEzspDongle::ashCbInfo(CAsh::EAshInfo info) {
	clogD <<  "ashCbInfo : " << CAsh::getEAshInfoAsString(info) << "\n";

	if (CAsh::EAshInfo::ASH_STATE_CHANGE == info) {
        // inform upper layer that dongle is ready !
        if( ash.isConnected() )
        {
            notifyObserversOfDongleState( DONGLE_READY );
            this->lastKnownMode = CEzspDongle::Mode::EZSP_NCP;    /* We are now sure the dongle is communicating over ASH */
        }
        else
        {
            notifyObserversOfDongleState( DONGLE_REMOVE );
        }
    }
	else if (CAsh::EAshInfo::ASH_NACK == info) {
        clogW << "Caught an ASH NACK from NCP... resending\n";
        wait_rsp = false;
        sendNextMsg();
    }
	else if (CAsh::EAshInfo::ASH_RESET_FAILED == info) {
        /* ASH reset failed */
        if (firstStartup)
        {
            /* If this is the startup sequence, we might be in bootloader prompt mode, not in ASH mode, so try to exit to EZSP/ASH mode from bootloader */
            if (this->switchToFirmwareUpgradeOnInitTimeout)
            {
                this->setMode(CEzspDongle::Mode::BOOTLOADER_FIRMWARE_UPGRADE);
            }
            else
            {
                this->setMode(CEzspDongle::Mode::BOOTLOADER_EXIT_TO_EZSP_NCP);
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
    NSSPI::ByteBuffer li_data;
    NSSPI::ByteBuffer lo_msg;

    li_data.clear();
    for( size_t loop=0; loop< dataLen; loop++ )
    {
        li_data.push_back(dataIn[loop]);
    }

    while( !li_data.empty())
    {
        if (this->lastKnownMode == CEzspDongle::Mode::EZSP_NCP || this->lastKnownMode == CEzspDongle::Mode::UNKNOWN) {
            lo_msg = ash.decode(li_data);

            /* Got an incoming EZSP message... will be forwarded to the user */
            if( !lo_msg.empty() )
            {
                size_t l_size;

                //clogD << "CEzspDongle::handleInputData ash message decoded" << std::endl;
                /* Extract the EZSP command and store it into l_cmd */
                EEzspCmd l_cmd = static_cast<EEzspCmd>(lo_msg.at(2));
                /* Payload will remain in buffer lo_msg */
                /* Remove the leading EZSP header from the payload */
                lo_msg.erase(lo_msg.begin(),lo_msg.begin()+3);  /* FIXME: make sure buffer is more than 2 bytes large */
                /* Remove the trailing EZSP CRC16 from the payload */
                lo_msg.erase(lo_msg.end()-2,lo_msg.end());  /* FIXME: make sure buffer is more than 2 bytes large */

                /* Send an EZSP ACK and unqueue messages, except for EZSP_LAUNCH_STANDALONE_BOOTLOADER that should not lead to any additional byte sent */
                if (l_cmd != EEzspCmd::EZSP_LAUNCH_STANDALONE_BOOTLOADER)
                {
                    NSSPI::ByteBuffer l_msg = ash.AckFrame();
                    this->uartHandle->write(l_size, l_msg.data(), l_msg.size());
                    /* Unqueue the message (and send a new one) if required */
                    this->handleResponse(l_cmd);
                }
                /* Notify the user(s) (via observers) about this incoming EZSP message */
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

void CEzspDongle::sendCommand(EEzspCmd i_cmd, NSSPI::ByteBuffer i_cmd_payload )
{
    SMsg l_msg;

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
    if (this->lastKnownMode != CEzspDongle::Mode::EZSP_NCP && this->lastKnownMode != CEzspDongle::Mode::UNKNOWN) {
        clogW << "Refusing to send EZSP messages in bootloader mode\n";
        return; /* No EZSP message can be sent in bootloader mode */
    }
    if( (!wait_rsp) && (!sendingMsgQueue.empty()) )
    {
        SMsg l_msg = sendingMsgQueue.front();

        // encode command using ash and write to uart
        NSSPI::ByteBuffer li_data;
        NSSPI::ByteBuffer l_enc_data;
        size_t l_size;

        li_data.push_back(static_cast<uint8_t>(l_msg.i_cmd));
        li_data.insert(li_data.end(), l_msg.payload.begin(), l_msg.payload.end() ); /* Append payload at the end of li_data */

        l_enc_data = ash.DataFrame(li_data);
		if (this->uartHandle) {
			//-- clogD << "CEzspDongle::sendCommand pUart->write" << std::endl;
			this->uartHandle->write(l_size, l_enc_data.data(), l_enc_data.size());

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

void CEzspDongle::setMode(CEzspDongle::Mode requestedMode) {
    if (this->lastKnownMode != CEzspDongle::Mode::EZSP_NCP
        && (requestedMode == CEzspDongle::Mode::EZSP_NCP || requestedMode == CEzspDongle::Mode::BOOTLOADER_EXIT_TO_EZSP_NCP)) {
        /* We are requested to get out of the booloader */
        this->lastKnownMode = requestedMode;
        this->blp.registerSerialWriteFunc([this](size_t& writtenCnt, const uint8_t* buf, size_t cnt) -> int {
            return this->uartHandle->write(writtenCnt, buf, cnt);
        });    /* Allow the blp object to write to the serial port via our own pUart attribute */
        this->blp.registerPromptDetectCallback([this]() {
            notifyObserversOfBootloaderPrompt();
            this->blp.selectModeRun(); /* As soon as we detect a bootloader prompt, we will request to run the application (EZSP NCP mode) */
            this->lastKnownMode = CEzspDongle::Mode::EZSP_NCP;   /* After launching the run command, we are in EZSP/ZSH mode */
            /* Restart the EZSP startup procedure here */
            this->reset();
        });
        this->blp.reset();    /* Reset the bootloader parser until we get a valid bootloader prompt */
        return;
    }
    if ((this->lastKnownMode == CEzspDongle::Mode::EZSP_NCP || this->lastKnownMode == CEzspDongle::Mode::UNKNOWN)
        && requestedMode == CEzspDongle::Mode::BOOTLOADER_FIRMWARE_UPGRADE) {
        clogD << "Attaching bootloader parser to serial port\n";
        /* We are requesting to switch from EZSP/ASH to bootloader parsing mode, and then perform a firmware upgrade */
        this->lastKnownMode = requestedMode;
        this->blp.registerSerialWriteFunc([this](size_t& writtenCnt, const uint8_t* buf, size_t cnt) -> int {
            return this->uartHandle->write(writtenCnt, buf, cnt);
        });    /* Allow the blp object to write to the serial port via our own pUart attribute */
        this->blp.registerPromptDetectCallback([this]() {
            notifyObserversOfBootloaderPrompt();
            /* Note: we provide selectModeUpgradeFw() with a callback that will be invoked when the firmware image transfer over serial link can start */
            /* This callback will only invoke our own notifyObserversOfFirmwareXModemXfrReady() method, that will in turn notify all observers that the firmware image transfer can start */
            this->blp.selectModeUpgradeFw([this]() { this->notifyObserversOfFirmwareXModemXfrReady(); });
            this->lastKnownMode = CEzspDongle::Mode::BOOTLOADER_FIRMWARE_UPGRADE;   /* After launching the upgrade command, we are in firmware upgrade mode (X-modem) */
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

void CEzspDongle::notifyObserversOfEzspRxMessage( EEzspCmd i_cmd, NSSPI::ByteBuffer i_message ) {
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
	/* Response to a command previously sent */
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
		SMsg l_msgQ = sendingMsgQueue.front();
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
