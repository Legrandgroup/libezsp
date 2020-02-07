/**
 * @file lib-ezsp-main.cpp
 */

#include "ezsp/lib-ezsp-main.h"
#include "spi/ILogger.h"
#include "ezsp/ezsp-protocol/get-network-parameters-response.h"  // For CGetNetworkParametersResponse
#include "ezsp/ezsp-protocol/struct/ember-key-struct.h"  // For CEmberKeyStruct

#include <sstream>
#include <iomanip>

namespace NSEZSP {
/**
 * @brief Internal states for CLibEzspMain
 * 
 * @note Not all these states are exposed to the outside, only CLibEzspState states (and the related changes) are notified
 *       A mapping between CLibEzspInternalState and CLibEzspMain can be found in method setState()
 */
enum class CLibEzspInternalState {
    UNINITIALIZED,                      /*<! Initial state, before starting */
    WAIT_DONGLE_READY,
    GETTING_EZSP_VERSION,               /*<! Inside the EZSP version matching loop */
    GETTING_XNCP_INFO,                  /*<! Inside the XNCP info check */
    STACK_INIT,                         /*<! We are starting up the Zigbee stack in the adapter */
    FORM_NWK_IN_PROGRESS,               /*<! We are currently creating a new Zigbee network */
    LEAVE_NWK_IN_PROGRESS,              /*<! We are currently leaving the Zigbee network we previously joined */
    READY,                              /*<! Library is ready to work and process new command */
    SCANNING,                           /*<! An network scan in currently being run */
    INIT_FAILED,                        /*<! Initialisation failed, Library is out of work */
    SINK_BUSY,                          /*<! Enclosed sink is busy executing commands */
    SWITCHING_TO_BOOTLOADER_MODE,       /*<! Switch to bootloader is pending */
    IN_BOOTLOADER_MENU,                 /*<! We are on the bootloader menu prompt */
    IN_XMODEM_XFR,                      /*<! We are currently doing X-Modem transfer */
    SWITCHING_TO_EZSP_MODE,             /*<! Switch to EZSP mode (normal mode) is pending */
    SWITCH_TO_BOOTLOADER_IN_PROGRESS,
};
} // namespace NSEZSP

using NSEZSP::CLibEzspMain;
using NSEZSP::CLibEzspState;
using NSEZSP::CLibEzspInternalState;

CLibEzspMain::CLibEzspMain(NSSPI::IUartDriver *uartDriver,
        NSSPI::TimerBuilder &timerbuilder,
        unsigned int requestZbNetworkResetToChannel) :
    timerbuilder(timerbuilder),
    exp_ezsp_min_version(6),    /* Expect EZSP version 6 minimum */
    exp_ezsp_max_version(7),    /* Expect EZSP version 7 maximum */
    exp_stack_type(2),  /* Expect a stack type=2 (mesh) */
    xncpManufacturerId(0),  /* 0 means unknown (yet) */
    xncpVersionNumber(0),  /* 0 means unknown (yet) */
    lib_state(CLibEzspInternalState::UNINITIALIZED),
    obsStateCallback(nullptr),
    dongle(timerbuilder, this),
    zb_messaging(dongle, timerbuilder),
    zb_nwk(dongle, zb_messaging),
    gp_sink(dongle, zb_messaging),
    obsGPFrameRecvCallback(nullptr),
    obsGPSourceIdCallback(nullptr),
    resetDot154ChannelAtInit(requestZbNetworkResetToChannel),
    scanInProgress(false),
    lastChannelToEnergyScan()
{
    // uart
    if( dongle.open(uartDriver) ) {
        clogI << "CLibEzspMain open success !" << std::endl;
        dongle.registerObserver(this);
        gp_sink.registerObserver(this);
        setState(CLibEzspInternalState::WAIT_DONGLE_READY);  /* Because the dongle observer has been set to ourselves just above, our handleDongleState() method will be called back as soon as the dongle is detected */
    }
}

void CLibEzspMain::forceFirmwareUpgradeOnInitTimeout()
{
    this->dongle.forceFirmwareUpgradeOnInitTimeout();
}

void CLibEzspMain::registerLibraryStateCallback(FLibStateCallback newObsStateCallback)
{
    this->obsStateCallback = newObsStateCallback;
}

void CLibEzspMain::registerGPFrameRecvCallback(FGpFrameRecvCallback newObsGPFrameRecvCallback)
{
    this->obsGPFrameRecvCallback = newObsGPFrameRecvCallback;
}

void CLibEzspMain::registerGPSourceIdCallback(FGpSourceIdCallback newObsGPSourceIdCallback)
{
    this->obsGPSourceIdCallback = newObsGPSourceIdCallback;
}

void CLibEzspMain::setState( CLibEzspInternalState i_new_state )
{
    this->lib_state = i_new_state;
    if( nullptr != obsStateCallback )
    {
        switch (i_new_state)
        {
            case CLibEzspInternalState::UNINITIALIZED:
            case CLibEzspInternalState::WAIT_DONGLE_READY:
            case CLibEzspInternalState::GETTING_EZSP_VERSION:
            case CLibEzspInternalState::GETTING_XNCP_INFO:
            case CLibEzspInternalState::STACK_INIT:
                obsStateCallback(CLibEzspState::UNINITIALIZED);
                break;
            case CLibEzspInternalState::READY:
            case CLibEzspInternalState::SCANNING:
                obsStateCallback(CLibEzspState::READY);
                break;
            case CLibEzspInternalState::INIT_FAILED:
                obsStateCallback(CLibEzspState::INIT_FAILED);
                break;
            case CLibEzspInternalState::SINK_BUSY:
            case CLibEzspInternalState::FORM_NWK_IN_PROGRESS:
            case CLibEzspInternalState::LEAVE_NWK_IN_PROGRESS:
                obsStateCallback(CLibEzspState::SINK_BUSY);
                break;
            case CLibEzspInternalState::SWITCHING_TO_BOOTLOADER_MODE:
            case CLibEzspInternalState::IN_BOOTLOADER_MENU:
            case CLibEzspInternalState::IN_XMODEM_XFR:
                obsStateCallback(CLibEzspState::SINK_BUSY);
                break;
            default:
                clogE << "Internal state can not be translated to public state\n";
        }
    }
}

CLibEzspInternalState CLibEzspMain::getState() const
{
    return this->lib_state;
}

void CLibEzspMain::dongleInit(uint8_t ezsp_version)
{
    /* First request stack protocol version using the related EZSP command
     * Note that we really need to send this specific command just after a reset because until we do, the dongle will refuse most other commands anyway
    /* Response to this should be the reception of an EZSP_VERSION in the handleEzspRxMessage() handler below
     */
    setState(CLibEzspInternalState::GETTING_EZSP_VERSION);
    dongle.sendCommand(EEzspCmd::EZSP_VERSION, NSSPI::ByteBuffer({ ezsp_version }));
}

void CLibEzspMain::getXncpInfo()
{
    setState(CLibEzspInternalState::GETTING_XNCP_INFO);
    dongle.sendCommand(EEzspCmd::EZSP_GET_XNCP_INFO);
}

void CLibEzspMain::stackInit()
{
    std::vector<SEzspConfig> l_config;

    l_config.push_back({.id = EZSP_CONFIG_NEIGHBOR_TABLE_SIZE,
                        .value = 32});
    l_config.push_back({.id = EZSP_CONFIG_APS_UNICAST_MESSAGE_COUNT,
                        .value = 10});
    l_config.push_back({.id = EZSP_CONFIG_BINDING_TABLE_SIZE,
                        .value = 0});
    l_config.push_back({.id = EZSP_CONFIG_ADDRESS_TABLE_SIZE,
                        .value = 64});
    l_config.push_back({.id = EZSP_CONFIG_MULTICAST_TABLE_SIZE,
                        .value = 8});
    l_config.push_back({.id = EZSP_CONFIG_ROUTE_TABLE_SIZE,
                        .value = 32});
    l_config.push_back({.id = EZSP_CONFIG_DISCOVERY_TABLE_SIZE,
                        .value = 16});
    l_config.push_back({.id = EZSP_CONFIG_STACK_PROFILE,
                        .value = 2});
    l_config.push_back({.id = EZSP_CONFIG_SECURITY_LEVEL,
                        .value = 5});
    l_config.push_back({.id = EZSP_CONFIG_MAX_HOPS,
                        .value = 15});
    l_config.push_back({.id = EZSP_CONFIG_MAX_END_DEVICE_CHILDREN,
                        .value = 32}); // define number of sleepy end device directly attached to dongle
    l_config.push_back({.id = EZSP_CONFIG_INDIRECT_TRANSMISSION_TIMEOUT,
                        .value = 3000});
    l_config.push_back({.id = EZSP_CONFIG_END_DEVICE_POLL_TIMEOUT,
                        .value = 5});
    l_config.push_back({.id = EZSP_CONFIG_MOBILE_NODE_POLL_TIMEOUT,
                        .value = 20});
    l_config.push_back({.id = EZSP_CONFIG_RESERVED_MOBILE_CHILD_ENTRIES,
                        .value = 0});
    l_config.push_back({.id = EZSP_CONFIG_TX_POWER_MODE,
                        .value = 0});
    l_config.push_back({.id = EZSP_CONFIG_DISABLE_RELAY,
                        .value = 0});
    l_config.push_back({.id = EZSP_CONFIG_TRUST_CENTER_ADDRESS_CACHE_SIZE,
                        .value = 0});
    l_config.push_back({.id = EZSP_CONFIG_SOURCE_ROUTE_TABLE_SIZE,
                        .value = 0});
    l_config.push_back({.id = EZSP_CONFIG_END_DEVICE_POLL_TIMEOUT_SHIFT,
                        .value = 6});
    l_config.push_back({.id = EZSP_CONFIG_FRAGMENT_WINDOW_SIZE,
                        .value = 0});
    l_config.push_back({.id = EZSP_CONFIG_FRAGMENT_DELAY_MS,
                        .value = 0});
    l_config.push_back({.id = EZSP_CONFIG_KEY_TABLE_SIZE,
                        .value = 12});
    l_config.push_back({.id = EZSP_CONFIG_APS_ACK_TIMEOUT,
                        .value = (50*30)+100});
    l_config.push_back({.id = EZSP_CONFIG_BEACON_JITTER_DURATION,
                        .value = 3});
    l_config.push_back({.id = EZSP_CONFIG_END_DEVICE_BIND_TIMEOUT,
                        .value = 60});
    l_config.push_back({.id = EZSP_CONFIG_PAN_ID_CONFLICT_REPORT_THRESHOLD,
                        .value = 1});
    l_config.push_back({.id = EZSP_CONFIG_REQUEST_KEY_TIMEOUT,
                        .value = 0});
    /*l_config.push_back({.id = EZSP_CONFIG_CERTIFICATE_TABLE_SIZE,
                        .value = 1});*/
    l_config.push_back({.id = EZSP_CONFIG_APPLICATION_ZDO_FLAGS,
                        .value =0});
    l_config.push_back({.id = EZSP_CONFIG_BROADCAST_TABLE_SIZE,
                        .value = 15});
    l_config.push_back({.id = EZSP_CONFIG_MAC_FILTER_TABLE_SIZE,
                        .value = 0});
    l_config.push_back({.id = EZSP_CONFIG_SUPPORTED_NETWORKS,
                        .value = 1});
    l_config.push_back({.id = EZSP_CONFIG_SEND_MULTICASTS_TO_SLEEPY_ADDRESS,
                        .value = 0});
    l_config.push_back({.id = EZSP_CONFIG_ZLL_GROUP_ADDRESSES,
                        .value = 0});
    /*l_config.push_back({.id = EZSP_CONFIG_ZLL_RSSI_THRESHOLD,
                        .value = -128});*/
    l_config.push_back({.id = EZSP_CONFIG_MTORR_FLOW_CONTROL,
                        .value = 1});
    l_config.push_back({.id = EZSP_CONFIG_RETRY_QUEUE_SIZE,
                        .value = 8});
    l_config.push_back({.id = EZSP_CONFIG_NEW_BROADCAST_ENTRY_THRESHOLD,
                        .value = 10});
    l_config.push_back({.id = EZSP_CONFIG_TRANSIENT_KEY_TIMEOUT_S,
                        .value = 300});
    l_config.push_back({.id = EZSP_CONFIG_BROADCAST_MIN_ACKS_NEEDED,
                        .value = 1});
    l_config.push_back({.id = EZSP_CONFIG_TC_REJOINS_USING_WELL_KNOWN_KEY_TIMEOUT_S,
                        .value = 600});
    l_config.push_back({.id = EZSP_CONFIG_PACKET_BUFFER_COUNT,
                        .value = 0xFF}); // use all remain memory for in/out radio packets

    std::vector<SEzspPolicy> l_policy;
    l_policy.push_back({.id = EZSP_TRUST_CENTER_POLICY,
                        .decision = EZSP_ALLOW_PRECONFIGURED_KEY_JOINS});
    l_policy.push_back({.id = EZSP_MESSAGE_CONTENTS_IN_CALLBACK_POLICY,
                        .decision = EZSP_MESSAGE_TAG_ONLY_IN_CALLBACK});
    l_policy.push_back({.id = EZSP_BINDING_MODIFICATION_POLICY,
                        .decision = EZSP_CHECK_BINDING_MODIFICATIONS_ARE_VALID_ENDPOINT_CLUSTERS});
    l_policy.push_back({.id = EZSP_POLL_HANDLER_POLICY,
                        .decision = EZSP_POLL_HANDLER_IGNORE});

    zb_nwk.stackInit(l_config, l_policy);
}

/**
 * Oberver handlers
 */
void CLibEzspMain::handleDongleState( EDongleState i_state )
{
    // clogI << __func__ << "() => dongleState : " << i_state << "\n";

    if( DONGLE_READY == i_state )
    {
        if( CLibEzspInternalState::WAIT_DONGLE_READY == getState() )
        {
            dongleInit(this->exp_ezsp_min_version);
        }
    }
    else if( DONGLE_REMOVE == i_state )
    {
        // TODO: manage this !
        clogW << __func__ << "() dongle removed\n";
    }
    else
    {
        clogD << __func__ << "() dongle state "<< i_state << std::endl;
    }
}

bool CLibEzspMain::clearAllGPDevices()
{
    if (this->getState() != CLibEzspInternalState::READY) {
        return false;
    }
    if (!this->gp_sink.gpClearAllTables())
    {
        return false; /* Probably sink is not ready */
    }

    this->setState(CLibEzspInternalState::SINK_BUSY);
    return true;
}

bool CLibEzspMain::removeGPDevices(std::vector<uint32_t>& sourceIdList)
{
    if (this->getState() != CLibEzspInternalState::READY) {
        return false;
    }
    if (!this->gp_sink.removeGpds(sourceIdList))
    {
        return false; /* Probably sink is not ready */
    }

    this->setState(CLibEzspInternalState::SINK_BUSY);
    return true;
}

bool CLibEzspMain::addGPDevices(const std::vector<CGpDevice> &gpDevicesList)
{
    if (this->getState() != CLibEzspInternalState::READY) {
        return false;
    }
    if (!this->gp_sink.registerGpds(gpDevicesList))
    {
        return false; /* Probably sink is not ready */
    }

    this->setState(CLibEzspInternalState::SINK_BUSY);
    return true;
}

bool CLibEzspMain::openCommissioningSession()
{
    if (this->getState() != CLibEzspInternalState::READY) {
        return false;
    }
    this->gp_sink.openCommissioningSession();
    return true;
}

bool CLibEzspMain::closeCommissioningSession()
{
    if (this->getState() != CLibEzspInternalState::READY) {
        return false;
    }
    gp_sink.closeCommissioningSession();
    return true;
}

void CLibEzspMain::setAnswerToGpfChannelRqstPolicy(bool allowed)
{
    this->gp_sink.authorizeAnswerToGpfChannelRqst(allowed);
}

void CLibEzspMain::setFirmwareUpgradeMode()
{
    this->dongle.sendCommand(EZSP_LAUNCH_STANDALONE_BOOTLOADER, { 0x01 });  /* 0x00 for STANDALONE_BOOTLOADER_NORMAL_MODE */
    this->setState(CLibEzspInternalState::SWITCHING_TO_BOOTLOADER_MODE);
    /* We should now receive an EZSP_LAUNCH_STANDALONE_BOOTLOADER in the handleEzspRxMessage() handler below, and only then, issue a carriage return to get the bootloader prompt */
}

bool CLibEzspMain::startEnergyScan(FEnergyScanCallback energyScanCallback, uint8_t duration)
{
    if (this->getState() != CLibEzspInternalState::READY || this->scanInProgress) {
        clogE << "Ignoring request for energy scan because we're still waiting for a previous scan to finish\n";
        return false;
    }
    NSSPI::ByteBuffer l_payload = { EZSP_ENERGY_SCAN };
    uint32_t channelMask = 0x07FFF800; // Range from channel 11 to 26 (inclusive)
    l_payload.push_back(u32_get_byte0(channelMask));
    l_payload.push_back(u32_get_byte1(channelMask));
    l_payload.push_back(u32_get_byte2(channelMask));
    l_payload.push_back(u32_get_byte3(channelMask));
    l_payload.push_back(duration);
    this->setState(CLibEzspInternalState::SCANNING);
    this->obsEnergyScanCallback = energyScanCallback;
    this->dongle.sendCommand(EZSP_START_SCAN, l_payload);
    return true;
}

bool CLibEzspMain::setChannel(uint8_t channel) {
    if (this->getState() != CLibEzspInternalState::READY || this->scanInProgress) {
        return false;
    }
    this->dongle.sendCommand(EZSP_SET_RADIO_CHANNEL, { channel });
}

void CLibEzspMain::handleFirmwareXModemXfr()
{
    this->setState(CLibEzspInternalState::IN_XMODEM_XFR);
    clogW << "EZSP adapter is now ready to receive a firmware image (.gbl) via X-modem\n";
    exit(0);
}

void CLibEzspMain::handleEzspRxMessage_VERSION(const NSSPI::ByteBuffer& i_msg_receive)
{
	uint8_t ezspProtocolVersion;
	uint8_t ezspStackType;
	uint16_t ezspStackVersion;
	std::stringstream bufDump;
	bool truncatedVersion = false;  /* Flag set to true when receiving a truncated EZSP_VERSION payload */
	bool acceptableVersion = false; /* Flag set to true when the version received from the EZSP adapter is acceptable for us */
	for (unsigned int loop=0; loop<i_msg_receive.size(); loop++) { bufDump << " " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[loop]); }
	clogD << "Got EZSP_VERSION payload:" << bufDump.str() << "\n";
	// Check if the wanted protocol version, and display stack version
	if (i_msg_receive.size() == 2)
	{
		clogW << "Got a truncated EZSP version frame from a buggy NCP, using only the 2 last bytes\n";
		ezspStackVersion = dble_u8_to_u16(i_msg_receive[1], i_msg_receive[0]);
		truncatedVersion = true;
		/* We assume the version is acceptable when receiving a truncated version, because truncated payloads only occur at the second attempt
		 * and this means we have already re-run dongleInit() below with the new expected version directly coming from the response of the adapter
		 */
		acceptableVersion = true;
		/* The two values below are not sent on a truncated version, we thus used the cached values from a previous run */
		ezspProtocolVersion = this->exp_ezsp_min_version;
		ezspStackType = this->exp_stack_type;
	}
	if (i_msg_receive.size() == 4)
	{
		ezspProtocolVersion = i_msg_receive.at(0);
		ezspStackType = i_msg_receive.at(1);
		ezspStackVersion = dble_u8_to_u16(i_msg_receive[3], i_msg_receive[2]);
		if (ezspStackType != this->exp_stack_type)
		{
			clogE << "Wrong stack type: " << static_cast<unsigned int>(ezspStackType) << ", expected: " << static_cast<unsigned int>(this->exp_stack_type) << "\n";
			clogE << "Stopping init here. Library will not work with this EZSP adapter\n";
			return;
		}
		if (ezspProtocolVersion > this->exp_ezsp_min_version || ezspProtocolVersion <= this->exp_ezsp_max_version)
		{
			clogW << "Current EZSP version supported by dongle (" << static_cast<int>(ezspProtocolVersion) << ") is higher than our minimum (" << static_cast<int>(exp_ezsp_min_version) << "). Re-initializing dongle\n";
			this->exp_ezsp_min_version = ezspProtocolVersion;
			acceptableVersion = false;
			dongleInit(this->exp_ezsp_min_version);
            return;
		}
		else if (ezspProtocolVersion == this->exp_ezsp_min_version &&  ezspProtocolVersion <= this->exp_ezsp_max_version)
		{
			acceptableVersion = true;
		}
	}
	if (acceptableVersion)
	{
		std::stringstream bufDump;  /* Log message container */
		// EZSP protocol version
		bufDump << "EZSP adapter is using EZSPv" << std::dec << static_cast<unsigned int>(ezspProtocolVersion);

		// Stack type
		bufDump << " with stack type " << static_cast<unsigned int>(ezspStackType);
		if (ezspStackType == 2)
		{
			bufDump << " (mesh)";
		}

		// Stack version (encoded in nibbles)
		bufDump << ". Stack version: ";
		bufDump << static_cast<unsigned int>((ezspStackVersion >> 12) & 0xf) << ".";
		bufDump << static_cast<unsigned int>((ezspStackVersion >> 8) & 0xf) << ".";
		bufDump << static_cast<unsigned int>((ezspStackVersion >> 4) & 0xf) << ".";
		bufDump << static_cast<unsigned int>((ezspStackVersion) & 0x0f) << "\n";

		/* Output the log message */
		clogI << bufDump.str();

		// Now request the XNCP version
		this->getXncpInfo();
	}
	else
	{
		clogI << "EZSP version " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[0]) << " is not supported !" << std::endl;
	}
}

void CLibEzspMain::handleEzspRxMessage_EZSP_GET_XNCP_INFO(const NSSPI::ByteBuffer& i_msg_receive)
{
    std::stringstream bufDump;
    for (unsigned int loop=0; loop<i_msg_receive.size(); loop++) { bufDump << " " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[loop]); }
    clogD << "Got EZSP_GET_XNCP_INFO payload:" << bufDump.str() << "\n";

    if (i_msg_receive.size() < 5)
    {
        clogE << "Wrong size for EZSP_GET_XNCP_INFO message: " << static_cast<unsigned int>(i_msg_receive.size()) << " bytes\n";
    }
    else
    {
        if (i_msg_receive[0] != EMBER_SUCCESS)
        {
            clogW << "EZSP_GET_XNCP_INFO failed\n";
        }
        else
        {
            this->xncpManufacturerId = dble_u8_to_u16(i_msg_receive[2], i_msg_receive[1]);
            this->xncpVersionNumber = dble_u8_to_u16(i_msg_receive[4], i_msg_receive[3]);
            clogI << "XNCP manufacturer: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(this->xncpManufacturerId)
                  << ", version: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(this->xncpVersionNumber) << "\n";
        }
    }
    // Now, configure and startup the adapter's embedded stack
    setState(CLibEzspInternalState::STACK_INIT);
    stackInit();
}

void CLibEzspMain::handleEzspRxMessage_NETWORK_STATE(const NSSPI::ByteBuffer& i_msg_receive)
{
	if (this->getState() != CLibEzspInternalState::STACK_INIT)
	{
		clogW << "Got EZSP_NETWORK_STATE with value " << static_cast<unsigned int>(i_msg_receive.at(0)) << " while not in STACK_INIT state... assuming stack has been initialized\n";
	}
	clogI << "handleEzspRxMessage_NETWORK_STATE getting EZSP_NETWORK_STATE=" << unsigned(i_msg_receive.at(0)) << " while CLibEzspInternalState=" << static_cast<unsigned int>(this->getState()) << "\n";
	if( EMBER_NO_NETWORK == i_msg_receive.at(0) )
	{
		// We create a network on the required channel
		if (this->getState() == CLibEzspInternalState::STACK_INIT)
		{
			clogI << "Creating new network on channel " << static_cast<unsigned int>(this->resetDot154ChannelAtInit) << "\n";
			zb_nwk.formHaNetwork(static_cast<uint8_t>(this->resetDot154ChannelAtInit));
			//set new state
			this->setState(CLibEzspInternalState::FORM_NWK_IN_PROGRESS);
			this->resetDot154ChannelAtInit = 0; /* Prevent any subsequent network re-creation */
		}
	}
	else
	{
		if ((this->getState() == CLibEzspInternalState::STACK_INIT) && (this->resetDot154ChannelAtInit != 0))
		{
			clogI << "Zigbee reset requested... Leaving current network\n";
			// leave current network
			zb_nwk.leaveNetwork();
			this->setState(CLibEzspInternalState::LEAVE_NWK_IN_PROGRESS);
		}
	}
}

void CLibEzspMain::handleEzspRxMessage_EZSP_LAUNCH_STANDALONE_BOOTLOADER(const NSSPI::ByteBuffer& i_msg_receive)
{
    if( this->getState() == CLibEzspInternalState::SWITCHING_TO_BOOTLOADER_MODE )
    {
        clogD << "Bootloader prompt mode is now going to start. Scheduling selection of the firmware upgrade option.\n";
        this->dongle.setMode(CEzspDongleMode::BOOTLOADER_FIRMWARE_UPGRADE);
    }
    else
    {
        clogE << "Unexpected switch over to bootloader mode. Further commands will fail\n";
    }
}

void CLibEzspMain::handleEzspRxMessage_STACK_STATUS_HANDLER(const NSSPI::ByteBuffer& i_msg_receive)
{
	EEmberStatus status = static_cast<EEmberStatus>(i_msg_receive.at(0));
	/* We handle EZSP_STACK_STATUS_HANDLER only if we are not currently leaving network.
	 * This is because EZSP adapter send spurious EMBER_NETWORK_UP or EMBER_NOT_JOINED while leaving the network.
	 * We will ignore all these until we get a EZSP_LEAVE_NETWORK message (see handler below)
	 */
	if (this->getState() != CLibEzspInternalState::LEAVE_NWK_IN_PROGRESS)
	{
		clogD << "CEZSP_STACK_STATUS_HANDLER status : " << CEzspEnum::EEmberStatusToString(status) << "\n";
		/* Note: we start the sink below only if network is up, but even if this is the case, we will not do it if we have been asked to reset the Zigbee network
		* Indeed, if the Zigbee network needs to be reset, we will first have to leave and re-create a network in the EZSP_NETWORK_STATE case below, and only then
		* will we get called again with EMBER_NETWORK_UP once the Zigbee network has been re-created */
		if ((EMBER_NETWORK_UP == status) && (this->resetDot154ChannelAtInit == 0))
		{
			this->setState(CLibEzspInternalState::SINK_BUSY);
			/* Create a sink state change callback to find out when the sink is ready */
			/* When the sink becomes ready, then libezsp will also switch to ready state */
			auto clibobs = [this](ESinkState& i_state) -> bool
			{
				clogD << "Underneath sink changed to state: " << std::dec << static_cast<unsigned int>(i_state) << ", current libezsp state: " << static_cast<unsigned int>(this->getState()) << "\n";
				if ((ESinkState::SINK_READY == i_state) &&
					(this->getState() == CLibEzspInternalState::SINK_BUSY)) {
				   this->setState(CLibEzspInternalState::READY);
				}
				return true;   /* Do not ask the caller to withdraw ourselves from the callback */
			};
			gp_sink.registerStateCallback(clibobs);
			gp_sink.init(); /* When sink is ready, callback clibobs will invoke setState() */
		}
		else
		{
			clogD << "Call EZSP_NETWORK_STATE\n";
			dongle.sendCommand(EZSP_NETWORK_STATE);
		}
	}
	else
	{
		clogD << "Ignoring CEZSP_STACK_STATUS_HANDLER status (while in network leave state): " << CEzspEnum::EEmberStatusToString(status) << "\n";
	}
}

void CLibEzspMain::handleEzspRxMessage(EEzspCmd i_cmd, NSSPI::ByteBuffer i_msg_receive)
{
    clogD << "CLibEzspMain::handleEzspRxMessage " << CEzspEnum::EEzspCmdToString(i_cmd) << std::endl;

    switch( i_cmd )
    {
        case EZSP_STACK_STATUS_HANDLER:
        {
            handleEzspRxMessage_STACK_STATUS_HANDLER(i_msg_receive);
        }
        break;
        case EZSP_GET_NETWORK_PARAMETERS:
        {
            CGetNetworkParametersResponse l_rsp(i_msg_receive);
            clogI << l_rsp.String() << std::endl;
        }
        break;
        case EZSP_GET_KEY:
        {
            EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));
            i_msg_receive.erase(i_msg_receive.begin());
            CEmberKeyStruct l_rsp(i_msg_receive);
            clogI << "EZSP_GET_KEY status : " << CEzspEnum::EEmberStatusToString(l_status) << ", " << l_rsp.String() << std::endl;
        }
        break;
        // case EZSP_GET_EUI64:
        // {
        //     // put eui64 on database for later use
        //     db.dongleEui64.clear();
        //     for( uint8_t loop=0; loop<EMBER_EUI64_BYTE_SIZE; loop++ )
        //     {
        //         db.dongleEui64.push_back(i_msg_receive.at(loop));
        //     }
        // }
        // break;
        case EEzspCmd::EZSP_VERSION:
        {
           handleEzspRxMessage_VERSION(i_msg_receive);
        }
        break;
        case EZSP_GET_XNCP_INFO:
        {
           handleEzspRxMessage_EZSP_GET_XNCP_INFO(i_msg_receive);
        }
        break;
        case EZSP_NETWORK_STATE:
        {
           handleEzspRxMessage_NETWORK_STATE(i_msg_receive);
        }
        break;
        case EZSP_LEAVE_NETWORK:
        {
            if (this->getState() != CLibEzspInternalState::LEAVE_NWK_IN_PROGRESS)
            {
                clogW << "Got EZSP_LEAVE_NETWORK while not in CLibEzspInternalState=LEAVE_NWK_IN_PROGRESS\n";
            }
            // Reset our current state to stack init
            this->setState(CLibEzspInternalState::STACK_INIT);
        }
        break;
        /*
        case EZSP_INCOMING_MESSAGE_HANDLER:
        {
            // the most important function where all zigbee incoming message arrive
            clogW << "Got an incoming Zigbee message (decoding not yet supported)\n";
        }
        break; */

        case EZSP_LAUNCH_STANDALONE_BOOTLOADER:
        {
           handleEzspRxMessage_EZSP_LAUNCH_STANDALONE_BOOTLOADER(i_msg_receive);
        }
        break;

        case EZSP_START_SCAN:
        {
            if (this->getState() != CLibEzspInternalState::SCANNING)
            {
                clogW << "Got a EZSP_START_SCAN message while not in SCANNING state\n";
            }
            this->scanInProgress = (EMBER_SUCCESS == i_msg_receive.at(0));
            if (this->scanInProgress) {
                clogD << "Scan started\n";
                this->lastChannelToEnergyScan.clear(); // clean previous energy scan result
            }
        }
        break;

        case EZSP_ENERGY_SCAN_RESULT_HANDLER:
        {
            if (this->getState() != CLibEzspInternalState::SCANNING)
            {
                clogW << "Got a EZSP_ENERGY_SCAN_RESULT_HANDLER message while not in SCANNING state\n";
            }
            uint8_t channel = i_msg_receive.at(0);
            int8_t rssi = static_cast<int8_t>(i_msg_receive.at(1));
            this->lastChannelToEnergyScan.emplace(channel, rssi);
            clogD << "EZSP_ENERGY_SCAN_RESULT_HANDLER: channel: " << std::dec << static_cast<unsigned int>(channel) << " rssi: " << static_cast<int>(rssi) << " dBm\n";
        }
        break;

        case EZSP_SCAN_COMPLETE_HANDLER:
        {
            if (this->getState() != CLibEzspInternalState::SCANNING)
            {
                clogW << "Got a EZSP_SCAN_COMPLETE_HANDLER message while not in SCANNING state\n";
            }
            else
            {
                clogD << "Scan finished\n";
            }
            this->scanInProgress = false;
            if (this->getState() == CLibEzspInternalState::SCANNING) {
                this->setState(CLibEzspInternalState::READY);
                if (this->obsEnergyScanCallback)
                {
                    this->obsEnergyScanCallback(this->lastChannelToEnergyScan);
                    this->obsEnergyScanCallback = nullptr;  /* Disable callback */
                }
            }
        }
        break;

        default:
        {
            /* DEBUG VIEW
            std::stringstream bufDump;

            for (size_t i =0; i<i_msg_receive.size(); i++) {
                bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[i]) << " ";
            }
            clogI << "CAppDemo::ezspHandler : " << bufDump.str() << std::endl;
            */
        }
        break;
    }
}

void CLibEzspMain::handleBootloaderPrompt()
{
    clogI << "CLibEzspMain::handleBootloaderPrompt\n";
}

void CLibEzspMain::handleRxGpFrame( CGpFrame &i_gpf )
{
    // Start DEBUG
    clogI << "CLibEzspMain::handleRxGpFrame gp frame : " << i_gpf << std::endl;

    if( nullptr != obsGPFrameRecvCallback )
    {
        obsGPFrameRecvCallback(i_gpf);
    }
}

void CLibEzspMain::handleRxGpdId( uint32_t &i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status )
{
    if( nullptr != obsGPSourceIdCallback )
    {
        obsGPSourceIdCallback(i_gpd_id, i_gpd_known, i_gpd_key_status);
    }
}
