/**
 * @file ezsp-dongle.cpp
 *
 * @brief Handles EZSP communication with an adapter over a serial link
 */
#include <sstream>
#include <iomanip>

#include <ezsp/byte-manip.h>
#include "ezsp-dongle.h"
#include "spi/ILogger.h"

DEFINE_ENUM(Mode, EZSP_DONGLE_MODE_LIST, NSEZSP::CEzspDongle);

using NSEZSP::CEzspDongle;

/**
 * Note: because i_timer_builder is a reference, we have the garantee that it is not pointing to null
 * But we store it inside a pointer and not a reference because we want to be able to update the value of timerBuilder when invoking swap()
 * and swap cannot be executed on const references but can be executed on non-const pointers to a const reference.
 */
CEzspDongle::CEzspDongle(const NSSPI::TimerBuilder& i_timer_builder, CEzspDongleObserver* ip_observer) :
	firstStartup(true),
	version(),
	lastKnownMode(CEzspDongle::Mode::UNKNOWN),
	switchToFirmwareUpgradeOnInitTimeout(false),
	timerBuilder(&i_timer_builder),
	uartHandle(nullptr),
	uartIncomingDataHandler(),
	ezspSeqNum(0),
	ash(static_cast<CAshCallback*>(this), *timerBuilder),
	blp(*timerBuilder),
	sendingMsgQueue(),
	wait_rsp(false),
	observers() {
	if (ip_observer) {
		registerObserver(ip_observer);
	}
	/* By default, no parsing is done on the adapter serial port */
	this->ash.disable();
	this->blp.disable();
	/* Register ourselves as an observer of EZSP frames decoded out of the ASH stream. These EZSP frames will be handled by handleInputData() */
	this->ash.registerObserver(this);
}

CEzspDongle::CEzspDongle(const CEzspDongle& other) :
	firstStartup(other.firstStartup),
	version(other.version),
	lastKnownMode(other.lastKnownMode),
	switchToFirmwareUpgradeOnInitTimeout(other.switchToFirmwareUpgradeOnInitTimeout),
	timerBuilder(other.timerBuilder),
	uartHandle(other.uartHandle),
	uartIncomingDataHandler(other.uartIncomingDataHandler),
	ezspSeqNum(other.ezspSeqNum),
	ash(static_cast<CAshCallback*>(this), *timerBuilder),
	blp(*timerBuilder),
	sendingMsgQueue(other.sendingMsgQueue),
	wait_rsp(other.wait_rsp),
	observers(other.observers) {
	/* By default, no parsing is done on the adapter serial port */
	this->ash.disable();
	this->blp.disable();
	/* Register ourselves as an observer of EZSP frames decoded out of the ASH stream. These EZSP frames will be handled by handleInputData() */
	this->ash.registerObserver(this);
}

CEzspDongle::~CEzspDongle() {
	this->ash.disable();
	this->blp.disable();
	this->ash.unregisterObserver(this);
}

void swap(CEzspDongle& first, CEzspDongle& second) {
	using std::swap;	// Enable ADL
	using ::swap;

	swap(first.firstStartup, second.firstStartup);
	swap(first.version, second.version);
	swap(first.lastKnownMode, second.lastKnownMode);
	swap(first.switchToFirmwareUpgradeOnInitTimeout, second.switchToFirmwareUpgradeOnInitTimeout);
	swap(first.timerBuilder, second.timerBuilder);
	swap(first.uartHandle, second.uartHandle);
	swap(first.uartIncomingDataHandler, second.uartIncomingDataHandler);
	swap(first.ezspSeqNum, second.ezspSeqNum);
	swap(first.ash, second.ash);
	swap(first.blp, second.blp);
	swap(first.sendingMsgQueue, second.sendingMsgQueue);
	swap(first.wait_rsp, second.wait_rsp);
	swap(first.observers, second.observers);
	/* Once we have swapped the members of the two instances... the two instances have actually been swapped */
}

void CEzspDongle::setUart(NSSPI::IUartDriverHandle uartHandle) {
	this->uartHandle = uartHandle;
	this->uartHandle->setIncomingDataHandler(&this->uartIncomingDataHandler); /* UART will send incoming bytes to the uartIncomingDataHandler member we hold as attribute */
	/* Allow ash and blp objects to read to read bytes from the serial port */
	this->ash.registerSerialReadObservable(&(this->uartIncomingDataHandler));   /* Ask ASH to observe our uartIncomingDataHandler observable so that it will be notified about incoming bytes */
	this->blp.registerSerialReadObservable(&(this->uartIncomingDataHandler));   /* Ask BLP to observe our uartIncomingDataHandler observable so that it will be notified about incoming bytes */
	/* Allow ash and blp objects to write to the serial port via our own uartHandle attribute */
	this->ash.registerSerialWriter(this->uartHandle);
	this->blp.registerSerialWriter(this->uartHandle);
}

NSSPI::GenericAsyncDataInputObservable* CEzspDongle::getSerialReadObservable() {
	return &(this->uartIncomingDataHandler);
}

bool CEzspDongle::reset() {
	NSSPI::ByteBuffer l_buffer;
	size_t l_size;

	this->ezspSeqNum = 0;	/* Start over using sequence number 0 */
	if (!this->uartHandle) {
		clogE << "No UART usable driver when invoking reset()\n";
		return false;
	}
	else {
		/* Send a ASH reset to the NCP */
		this->blp.disable();
		this->ash.enable();
		if (!this->ash.sendResetNCPFrame()) {
			clogE << "Failed sending reset frame to serial port\n";
			return false;
		}
		else {
			clogD << "CEzspDongle UART reset\n";
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

void CEzspDongle::ashCbInfo(AshCodec::EAshInfo info) {
	clogD <<  "ashCbInfo : " << AshCodec::getEAshInfoAsString(info) << "\n";

	switch (info) {
	case AshCodec::EAshInfo::ASH_STATE_CONNECTED: {
		notifyObserversOfDongleState(DONGLE_READY);
		this->lastKnownMode = CEzspDongle::Mode::EZSP_NCP;    /* We are now sure the dongle is communicating over ASH */
	}
	break;
	case AshCodec::EAshInfo::ASH_STATE_DISCONNECTED: {
		notifyObserversOfDongleState(DONGLE_REMOVE);
	}
	break;
	case AshCodec::EAshInfo::ASH_NACK: {
		clogW << "Caught an ASH NACK from NCP... resending\n";
		wait_rsp = false;
		sendNextMsg();
	}
	break;
	case AshCodec::EAshInfo::ASH_RESET_FAILED: {
		/* ASH reset failed */
		if (firstStartup) {
			/* If this is the startup sequence, we might be in bootloader prompt mode, not in ASH mode, so try to exit to EZSP/ASH mode from bootloader */
			if (this->switchToFirmwareUpgradeOnInitTimeout) {
				this->setMode(CEzspDongle::Mode::BOOTLOADER_FIRMWARE_UPGRADE);
			}
			else {
				this->setMode(CEzspDongle::Mode::BOOTLOADER_EXIT_TO_EZSP_NCP);
			}
			firstStartup = false;
		}
		else {
			clogE << "EZSP adapter is not responding\n";
			notifyObserversOfDongleState( DONGLE_NOT_RESPONDING );
		}
	}
	break;
	default:
		clogW << "Caught an unknown ASH\n";
	}
}

void CEzspDongle::handleInputData(const unsigned char* dataIn, const size_t dataLen) {
	if (this->lastKnownMode != CEzspDongle::Mode::EZSP_NCP && this->lastKnownMode != CEzspDongle::Mode::UNKNOWN) {
		clogE << "EZSP message recevied while in bootloader prompt mode... Should not reach here\n";
		/* In bootloader parsing mode, incoming bytes are read directly by the bootloader prompt driver from the serial port */
		/* Bootloader decoder state changes are handled by callbacks we register on the bootloader prompt driver, no data payload is received asynchronously here */
		return;
	}

	NSSPI::ByteBuffer ezspMessage(dataIn, dataLen);
	EEzspCmd l_cmd;

	//clogD << "NCP->host EZSP message " << ezspMessage << "\n";

	/* Note: this code will handle all successfully decoded incoming EZSP messages */
	/* It won't be run in bootloader prompt mode, because the ASH driver is then disabled */

	if (this->knownEzspProtocolVersionGE(8)) {	/* EZSPv8 and higher */
		if (ezspMessage.size() < 5) {	/* EZSPv8 message should contain at least 5 bytes for v8 frames (see protocol format below) */
			clogE << "EZSP message is too short\n";
			return;
		}

		/* Silabs' document ug100-ezsp-reference-guide mentions, for EZSP starting from v7, in section 3 Protocol Format, that the EZSP frame format is:
		* Sequence (1 byte) | Frame Control Low Byte (1 byte) | Frame Control Hi Byte (1 byte) | Frame ID (2 byte) | Parameters (n bytes)
		*/

		/* Extract the EZSP command (frame ID) and store it into l_cmd */
		if (ezspMessage.at(4) != 0) {
			clogE << "Unsupported EZSPv8 frame ID (>0xff): 0x" << std::hex << std::setw(2) << std::setfill('0')
			      << static_cast<unsigned int>(ezspMessage.at(4))
			      << static_cast<unsigned int>(ezspMessage.at(3)) << "\n";
			return;
		}
		l_cmd = static_cast<EEzspCmd>(ezspMessage.at(3));
		/* Remove the leading EZSP header from the payload */
		ezspMessage.erase(ezspMessage.begin(), ezspMessage.begin()+5);
		/* Payload (frame parameters in Silabs' terminology) will remain in buffer ezspMessage */
	}
	else {	/* Unknown EZSP version or version strictly lower than v8 */
		if (ezspMessage.size() < 4) {	/* EZSP messages (v6 & v7) should contain at least 4 bytes for legacy frames (see protocol format below) */
			clogE << "EZSP message is too short\n";
			return;
		}

		/* Silabs' document ug100-ezsp-reference-guide mentions, for EZSP up to v7, in section 3 Protocol Format, that the EZSP frame format is:
		* Sequence (1 byte) | Frame Control (1 byte) | Legacy Frame ID (1 byte, almost always 0xFF) | Extended Frame Control (1 byte) | Frame ID (1 byte) | Parameters (n bytes)
		* Thus, in case we get a legacy frame ID at offset 2, we just get rid of both "Legacy Frame ID" and "Extended Frame Control" and get a frame formatted as legacy frames
		*/
		if (ezspMessage.size() >= 3 && ezspMessage.at(2) == 0xffU) { /* 0xff as frame ID means we use an extended header, where frame ID will actually be shifted 2 bytes away... so we just delete those two bytes */
			if (ezspMessage.size() < 4) {	/* We got Sequence+FC+Legacy indicating an extended FC... but there was nothing more! */
				clogE << "Truncated extended header in EZSP message\n";
				return;
			}
			ezspMessage.erase(ezspMessage.begin()+2, ezspMessage.begin()+4); /* Remove Legacy Frame ID + Extended Frame Control (offset+4 is kept as per begin() usage conventions)*/
		}

		/* EZSP message should now contain at least 3 bytes for all frames (reduced to legacy format):
		* Sequence (1 byte) | Frame Control (1 byte) | Frame ID (1 byte) | Parameters (n bytes)
		*/

		if (ezspMessage.size() < 3) {	/* EZSP message should contain at least 1 byte for sequence, 1 byte for frame control and a message ID field (1 or 2 bytes) */
			clogE << "EZSP message is too short\n";
			return;
		}
		/* Extract the EZSP command (frame ID) and store it into l_cmd */
		l_cmd = static_cast<EEzspCmd>(ezspMessage.at(2));
		/* Remove the leading EZSP header from the payload */
		ezspMessage.erase(ezspMessage.begin(), ezspMessage.begin()+3);
		/* Payload (frame parameters in Silabs' terminology) will remain in buffer ezspMessage */
	}
	/* Got an correct incoming EZSP message... will be forwarded to the user */

	//clogD << "Received EZSP message payload " << ezspMessage << "\n";

	/* Send an EZSP ACK and unqueue messages, except for EZSP_LAUNCH_STANDALONE_BOOTLOADER that should not lead to any additional byte sent */
	if (l_cmd != EEzspCmd::EZSP_LAUNCH_STANDALONE_BOOTLOADER) {
		this->ash.sendAckFrame();
		this->handleResponse(l_cmd); /* Unqueue the message (and send the next one) if required */
	}
	/* Notify the user(s) (via observers) about this incoming EZSP message */
	notifyObserversOfEzspRxMessage(l_cmd, ezspMessage);
}

void CEzspDongle::sendCommand(EEzspCmd i_cmd, NSSPI::ByteBuffer i_cmd_payload ) {
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

void CEzspDongle::sendNextMsg( void ) {
	if (this->lastKnownMode != CEzspDongle::Mode::EZSP_NCP && this->lastKnownMode != CEzspDongle::Mode::UNKNOWN) {
		clogW << "Refusing to send EZSP messages in bootloader mode\n";
		return; /* No EZSP message can be sent in bootloader mode */
	}

	if( (!wait_rsp) && (!sendingMsgQueue.empty()) ) {
		SMsg l_msg = sendingMsgQueue.front();

		clogD << "Sending to NCP EZSP command: " << CEzspEnum::EEzspCmdToString(l_msg.i_cmd) << " with payload " << l_msg.payload << "\n";
		NSSPI::ByteBuffer ezspMessage;

		// First, place the EZSP seq number byte
		ezspMessage.push_back(this->ezspSeqNum++);

		// Then, append the EZSP frame control byte (0x00)
		ezspMessage.push_back(0x00U);
		if (this->knownEzspProtocolVersionGE(8)) {
			ezspMessage.push_back(0x01U);	/* Frame format version 1 */
		}

		if (l_msg.i_cmd != NSEZSP::EEzspCmd::EZSP_VERSION && this->knownEzspProtocolVersionLT(8)) {
			/* For all EZSPv6 or EZSPv7 frames except "VersionRequest" frame, force an extended header 0xff 0x00 */
			ezspMessage.push_back(0xFFU);
			ezspMessage.push_back(0x00U);
		}

		ezspMessage.push_back(static_cast<uint8_t>(l_msg.i_cmd));
		if (this->knownEzspProtocolVersionGE(8)) {
			ezspMessage.push_back(0x00);
		}
		ezspMessage.append(l_msg.payload); /* Append payload at the end of li_data */

		//clogD << "host->NCP EZSP message " << ezspMessage << "\n";

		if (this->ash.sendDataFrame(ezspMessage)) {
			this->wait_rsp = true;
		}
	}
}


/**
 * Managing Observer of this class
 */
bool CEzspDongle::registerObserver(CEzspDongleObserver* observer) {
	return this->observers.emplace(observer).second;
}

bool CEzspDongle::unregisterObserver(CEzspDongleObserver* observer) {
	return static_cast<bool>(this->observers.erase(observer));
}

void CEzspDongle::forceFirmwareUpgradeOnInitTimeout() {
	this->switchToFirmwareUpgradeOnInitTimeout = true;
}

void CEzspDongle::setMode(CEzspDongle::Mode requestedMode) {
	if (this->lastKnownMode != CEzspDongle::Mode::EZSP_NCP
	        && (requestedMode == CEzspDongle::Mode::EZSP_NCP || requestedMode == CEzspDongle::Mode::BOOTLOADER_EXIT_TO_EZSP_NCP)) {
		/* We are requested to get out of the booloader */
		this->lastKnownMode = requestedMode;
		/* Allow the blp object to write to the serial port via our own pUart attribute */
		this->blp.registerPromptDetectCallback([this]() {
			notifyObserversOfBootloaderPrompt();
			this->blp.selectModeRun(); /* As soon as we detect a bootloader prompt, we will request to run the application (EZSP NCP mode) */
			this->lastKnownMode = CEzspDongle::Mode::EZSP_NCP;   /* After launching the run command, we are in EZSP/ZSH mode */
			this->ash.enable();	/* Enable ASH driver */
			this->blp.disable();	/* Disable BLP driver */
			/* Restart the EZSP startup procedure here */
			this->reset();
		});
		this->blp.enable();
		this->blp.reset();    /* Reset the bootloader parser until we get a valid bootloader prompt */
		return;
	}
	if ((this->lastKnownMode == CEzspDongle::Mode::EZSP_NCP || this->lastKnownMode == CEzspDongle::Mode::UNKNOWN)
	        && requestedMode == CEzspDongle::Mode::BOOTLOADER_FIRMWARE_UPGRADE) {
		clogD << "Attaching bootloader parser to serial port\n";
		/* We are requesting to switch from EZSP/ASH to bootloader parsing mode, and then perform a firmware upgrade */
		this->lastKnownMode = requestedMode;
		this->ash.disable();	/* Disable ASH driver */
		this->blp.enable();	/* Enable BLP driver */
		/* Allow the blp object to write to the serial port via our own pUart attribute */
		this->blp.registerPromptDetectCallback([this]() {
			notifyObserversOfBootloaderPrompt();
			/* Note: we provide selectModeUpgradeFw() with a callback that will be invoked when the firmware image transfer over serial link can start */
			/* This callback will only invoke our own notifyObserversOfFirmwareXModemXfrReady() method, that will in turn notify all observers that the firmware image transfer can start */
			this->blp.selectModeUpgradeFw([this]() {
				this->notifyObserversOfFirmwareXModemXfrReady();
			});
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

void CEzspDongle::handleDongleState( EDongleState i_state ) {
	// do nothing
}

void CEzspDongle::handleResponse( EEzspCmd i_cmd ) {
	/* Response to a command previously sent */
	if( !sendingMsgQueue.empty() ) {
		if (!wait_rsp) {
			/* If wait_rsp is false, we are not expecting a response to a previous command.
			   But sendingMsgQueue should always contain (at front) the last command sent without reply, so when sendingMsgQueue is not empty,
			   wait_rsp should be true
			*/
			clogE << "Received a message with a non-empty queue while no response was expected\n";
		}
		SMsg l_msgQ = sendingMsgQueue.front();
		if( l_msgQ.i_cmd == i_cmd ) { /* Make sure that the EZSP message is a response to the last command we sent */
			// remove waiting message and send next
			sendingMsgQueue.pop();
			wait_rsp = false;
			sendNextMsg();
		}    // response to a sending command
		else {
			clogE << "Asynchronous received EZSP message\n";
		}
	}
}

bool CEzspDongle::knownEzspProtocolVersion() const {
	/* Any value other than 0 is considered valid and a proof that we know which version is running on the adapter */
	return (this->version.ezspProtocolVersion != 0);
}

bool CEzspDongle::knownEzspProtocolVersionGE(uint8_t minIncludedVersion) const {
	if (!(this->knownEzspProtocolVersion())) {
		return false;	/* If we don't know the EZSP version, always return false */
	}
	return (this->version.ezspProtocolVersion >= minIncludedVersion);
}

bool CEzspDongle::knownEzspProtocolVersionLT(uint8_t maxExcludedVersion) const {
	if (!(this->knownEzspProtocolVersion())) {
		return false;	/* If we don't know the EZSP version, always return false */
	}
	return (this->version.ezspProtocolVersion < maxExcludedVersion);
}

CEzspDongle& CEzspDongle::operator=(CEzspDongle other) {
	swap(*this, other);
	return *this;
}
