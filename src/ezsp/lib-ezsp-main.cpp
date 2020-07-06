/**
 * @file lib-ezsp-main.cpp
 * 
 * @brief Main API methods for libezsp
 */

#include <sstream>
#include <iomanip>

#include <ezsp/ezsp-adapter-version.h>

#include "ezsp/lib-ezsp-main.h"
#include "spi/ILogger.h"
#include "spi/ITimer.h"
#include "ezsp/ezsp-protocol/get-network-parameters-response.h"  // For CGetNetworkParametersResponse
#include "ezsp/ezsp-protocol/struct/ember-key-struct.h"  // For CEmberKeyStruct

DEFINE_ENUM(State, CLIBEZSP_INTERNAL_STATE_LIST, NSEZSP::CLibEzspInternal);

using NSEZSP::CLibEzspMain;
using NSEZSP::CLibEzspPublic;
using NSEZSP::CLibEzspInternal;

CLibEzspMain::CLibEzspMain(NSSPI::IUartDriverHandle uartHandle, const NSSPI::TimerBuilder& timerbuilder, unsigned int requestZbNetworkResetToChannel) :
    uartHandle(uartHandle),
    timerbuilder(timerbuilder),
    exp_ezsp_min_version(6),    /* Expect EZSP version 6 minimum */
    exp_ezsp_max_version(8),    /* Expect EZSP version 8 maximum */
    exp_stack_type(2),  /* Expect a stack type=2 (mesh) */
    xncpManufacturerId(0),  /* 0 means unknown (yet) */
    xncpVersionNumber(0),  /* 0 means unknown (yet) */
    lib_state(CLibEzspInternal::State::UNINITIALIZED),
    obsStateCallback(nullptr),
    dongle(timerbuilder, this),
    zb_messaging(dongle, timerbuilder),
    zb_nwk(dongle, zb_messaging),
    gp_sink(dongle, zb_messaging),
    obsGPFrameRecvCallback(nullptr),
    obsGPSourceIdCallback(nullptr),
    resetDot154ChannelAtInit(requestZbNetworkResetToChannel),
    scanInProgress(false),
    lastChannelToEnergyScan(),
    registeredSourceIdsStats()
{
}

void CLibEzspMain::start()
{
    if (this->lib_state != CLibEzspInternal::State::UNINITIALIZED) {
        clogW << "Start invoked while already initialized\n";
    }
    if (this->uartHandle == nullptr) {
        clogW << "Start invoked without an effective UART driver\n";
    }
    this->dongle.registerObserver(this);
    this->gp_sink.registerObserver(this);
    this->dongle.setUart(this->uartHandle);
    if (this->dongle.reset()) {
        clogI << "EZSP serial communication started\n";
        setState(CLibEzspInternal::State::WAIT_DONGLE_READY);  /* Because the dongle observer has been set to ourselves just above, our handleDongleState() method will be called back as soon as the dongle is detected */
    }
    else {
        clogE << "EZSP failed starting serial communication with adapter\n";
        this->dongle.unregisterObserver(this);
        this->gp_sink.registerObserver(this);
    }
}

NSSPI::GenericAsyncDataInputObservable* CLibEzspMain::getAdapterSerialReadObservable() {
    return this->dongle.getSerialReadObservable();
}

NSEZSP::EzspAdapterVersion CLibEzspMain::getAdapterVersion() const {
	return this->dongle.getVersion();
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

void CLibEzspMain::setState(CLibEzspInternal::State i_new_state)
{
    CLibEzspInternal::State l_old_state = this->lib_state;
    this->lib_state = i_new_state;
	clogD << "CLibEzspMain state changing from " << NSEZSP::CLibEzspInternal::getStateAsString(l_old_state) << " to " << NSEZSP::CLibEzspInternal::getStateAsString(i_new_state) << "\n";
	if (nullptr != obsStateCallback) {
        switch (i_new_state)
        {
            case CLibEzspInternal::State::UNINITIALIZED:
            case CLibEzspInternal::State::WAIT_DONGLE_READY:
            case CLibEzspInternal::State::GETTING_EZSP_VERSION:
            case CLibEzspInternal::State::GETTING_XNCP_INFO:
            case CLibEzspInternal::State::STACK_INIT:
                obsStateCallback(CLibEzspPublic::State::UNINITIALIZED);
                break;
            case CLibEzspInternal::State::READY:
            case CLibEzspInternal::State::SCANNING:
                obsStateCallback(CLibEzspPublic::State::READY);
                break;
            case CLibEzspInternal::State::INIT_FAILED:
                obsStateCallback(CLibEzspPublic::State::INIT_FAILED);
                break;
            case CLibEzspInternal::State::SINK_BUSY:
            case CLibEzspInternal::State::FORM_NWK_IN_PROGRESS:
            case CLibEzspInternal::State::LEAVE_NWK_IN_PROGRESS:
                obsStateCallback(CLibEzspPublic::State::SINK_BUSY);
                break;
            case CLibEzspInternal::State::SWITCHING_TO_BOOTLOADER_MODE:
            case CLibEzspInternal::State::IN_BOOTLOADER_MENU:
                obsStateCallback(CLibEzspPublic::State::SINK_BUSY);
                break;
            case CLibEzspInternal::State::IN_XMODEM_XFR:
                obsStateCallback(CLibEzspPublic::State::IN_XMODEM_XFR);
                break;
            default:
                clogE << "Internal state can not be translated to public state\n";
        }
    }
}

CLibEzspInternal::State CLibEzspMain::getState() const
{
    return this->lib_state;
}

void CLibEzspMain::dongleInit(uint8_t ezsp_version)
{
    /* First request stack protocol version using the related EZSP command
     * Note that we really need to send this specific command just after a reset because until we do, the dongle will refuse most other commands anyway
    /* Response to this should be the reception of an EZSP_VERSION in the handleEzspRxMessage() handler below
     */
    setState(CLibEzspInternal::State::GETTING_EZSP_VERSION);
    dongle.sendCommand(EEzspCmd::EZSP_VERSION, NSSPI::ByteBuffer({ ezsp_version }));
}

void CLibEzspMain::getXncpInfo()
{
    setState(CLibEzspInternal::State::GETTING_XNCP_INFO);
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
        if( CLibEzspInternal::State::WAIT_DONGLE_READY == getState() )
        {
            dongleInit(this->exp_ezsp_min_version);
        }
    }
    else if( DONGLE_REMOVE == i_state )
    {
        // TODO: manage this !
        clogW << __func__ << "() dongle removed\n";
    }
	else if (i_state == DONGLE_VERSION_RETRIEVED) {
		NSEZSP::EzspAdapterVersion ezspAdapterVersion = this->dongle.getVersion();
		if (ezspAdapterVersion.xncpManufacturerId != static_cast<uint16_t>(NSEZSP::EzspAdapterVersion::Manufacturer::UNKNOWN)) {
			/* Note: here we only care about DONGLE_VERSION_RETRIEVED notifications if the manufacturer ID is known, meaning we have received both
			 * EZSP VERSION and XNCP INFO data
			 */
			if (ezspAdapterVersion.xncpManufacturerId != static_cast<unsigned int>(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND)) {
				clogW << "EZSP adapter is not from Legrand (manufacturer " << std::hex << static_cast<unsigned int>(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND) << " expected)\n";
			}
			else {
				clogI << "Legrand EZSP adapter found with hardware version " << std::dec << ezspAdapterVersion.xncpAdapterHardwareVersion
				      << " and firmware v" << ezspAdapterVersion.getFirmwareVersionAsString() << ", running EZSPv" << ezspAdapterVersion.ezspProtocolVersion << " with stack v" << ezspAdapterVersion.getStackVersionAsString() << "\n";
			}
        }
	}
	else {
		clogD << __func__ << "() dongle state "<< i_state << std::endl;
	}
}

bool CLibEzspMain::clearAllGPDevices()
{
    if (this->getState() != CLibEzspInternal::State::READY) {
        return false;
    }
    this->setState(CLibEzspInternal::State::SINK_BUSY);
    bool success = this->gp_sink.clearAllGpds();
    if (!success) {
        return false; /* Probably sink is not ready */
    }
    else {
        this->registeredSourceIdsStats.clear();
        return true;
    }
}

bool CLibEzspMain::removeGPDevices(const std::vector<uint32_t>& sourceIdList)
{
    if (this->getState() != CLibEzspInternal::State::READY) {
        return false;
    }
    this->setState(CLibEzspInternal::State::SINK_BUSY);
    if (!this->gp_sink.removeGpds(sourceIdList))
    {
        return false; /* Probably sink is not ready */
    }

    for (auto it = sourceIdList.begin(); it != sourceIdList.end(); ++it) {
        this->registeredSourceIdsStats.erase(*it);
    }
    return true;
}

bool CLibEzspMain::addGPDevices(const std::vector<CGpDevice> &gpDevicesList)
{
    if (this->getState() != CLibEzspInternal::State::READY) {
        return false;
    }
    this->setState(CLibEzspInternal::State::SINK_BUSY);
    if (!this->gp_sink.registerGpds(gpDevicesList))
    {
        return false; /* Probably sink is not ready */
    }

    for (auto it = gpDevicesList.begin(); it != gpDevicesList.end(); ++it) {
        this->registeredSourceIdsStats.insert(std::pair<uint32_t,NSEZSP::Stats::SourceIdData>(it->getSourceId(),NSEZSP::Stats::SourceIdData()));
    }
    return true;
}

bool CLibEzspMain::openCommissioningSession()
{
    if (this->getState() != CLibEzspInternal::State::READY) {
        return false;
    }
    this->gp_sink.openCommissioningSession();
    return true;
}

bool CLibEzspMain::closeCommissioningSession()
{
    if (this->getState() != CLibEzspInternal::State::READY) {
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
    this->setState(CLibEzspInternal::State::SWITCHING_TO_BOOTLOADER_MODE);
    /* We should now receive an EZSP_LAUNCH_STANDALONE_BOOTLOADER in the handleEzspRxMessage() handler below, and only then, issue a carriage return to get the bootloader prompt */
}

bool CLibEzspMain::startEnergyScan(FEnergyScanCallback energyScanCallback, uint8_t duration)
{
    if (this->getState() != CLibEzspInternal::State::READY || this->scanInProgress) {
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
    this->setState(CLibEzspInternal::State::SCANNING);
    this->obsEnergyScanCallback = energyScanCallback;
    this->dongle.sendCommand(EZSP_START_SCAN, l_payload);
    return true;
}

bool CLibEzspMain::setChannel(uint8_t channel) {
    if (this->getState() != CLibEzspInternal::State::READY || this->scanInProgress) {
        return false;
    }
    this->dongle.sendCommand(EZSP_SET_RADIO_CHANNEL, { channel });
    return true;
}

void CLibEzspMain::handleFirmwareXModemXfr()
{
    this->setState(CLibEzspInternal::State::IN_XMODEM_XFR);
    clogW << "EZSP adapter is now ready to receive a firmware image (.gbl) via X-modem\n";
}

void CLibEzspMain::handleEzspRxMessage_VERSION(const NSSPI::ByteBuffer& i_msg_receive) {
	std::stringstream bufDump;
	bool truncatedVersion = false;  /* Flag set to true when receiving a truncated EZSP_VERSION payload */
	bool acceptableVersion = false; /* Flag set to true when the version received from the EZSP adapter is acceptable for us */
	clogD << "Got EZSP_VERSION payload:" << NSSPI::Logger::byteSequenceToString(i_msg_receive) << "\n";
	// Check if the wanted protocol version, and display stack version
	if (i_msg_receive.size() == 2) {
		clogW << "Got a truncated EZSP version frame from a buggy NCP, using only the 2 last bytes\n";
		uint16_t ezspStackVersion = dble_u8_to_u16(i_msg_receive[1], i_msg_receive[0]);
		this->dongle.setFetchedEzspVersionData(ezspStackVersion);
		truncatedVersion = true;
		/* We assume the version is acceptable when receiving a truncated version, because truncated payloads only occur at the second attempt
		 * and this means we have already re-run dongleInit() below with the new expected version directly coming from the response of the adapter
		 */
		acceptableVersion = true;
		/* The two values below are not sent on a truncated version, but we use the values cached inside this->dongle from a previous run */
	}
	if (i_msg_receive.size() == 4) {
		uint8_t ezspProtocolVersion = i_msg_receive.at(0);
		uint8_t ezspStackType = i_msg_receive.at(1);
		uint16_t ezspStackVersion = dble_u8_to_u16(i_msg_receive[3], i_msg_receive[2]);
		this->dongle.setFetchedEzspVersionData(ezspStackVersion, ezspProtocolVersion, ezspStackType);
		if (ezspStackType != this->exp_stack_type) {
			clogE << "Wrong stack type: " << static_cast<unsigned int>(ezspStackType) << ", expected: " << static_cast<unsigned int>(this->exp_stack_type) << "\n";
			clogE << "Stopping init here. Library will not work with this EZSP adapter\n";
			return;
		}
		if (ezspProtocolVersion > this->exp_ezsp_min_version) {
			if (ezspProtocolVersion > this->exp_ezsp_max_version) {
				clogE << "Unsupported EZSP version (v" << std::dec << static_cast<int>(ezspProtocolVersion) << " is too high for this library)\n";
				acceptableVersion = false;
				return;
			}
			else {
				clogD << "Current EZSP version supported by dongle (" << std::dec << static_cast<int>(ezspProtocolVersion) << ") is higher than our minimum (" << static_cast<int>(exp_ezsp_min_version) << "). Re-initializing dongle\n";
				this->exp_ezsp_min_version = ezspProtocolVersion;
				acceptableVersion = false;
				dongleInit(this->exp_ezsp_min_version);
				return;
			}
		}
		else if (ezspProtocolVersion == this->exp_ezsp_min_version &&  ezspProtocolVersion <= this->exp_ezsp_max_version) {
			acceptableVersion = true;
		}
		else {
			acceptableVersion = false;
		}
	}
	if (acceptableVersion) {
		std::stringstream bufDump;  /* Log message container */
		NSEZSP::EzspAdapterVersion ezspVersionDetails = this->dongle.getVersion();
		std::string humanReadableStackType("");
		if (ezspVersionDetails.ezspStackType == 2) {
			humanReadableStackType = " (mesh)";
		}

		/* Output the log message */
		clogI << "EZSP adapter version is supported: EZSPv" << std::dec << static_cast<unsigned int>(ezspVersionDetails.ezspProtocolVersion)
		      << " with stack type " << static_cast<unsigned int>(ezspVersionDetails.ezspStackType) << humanReadableStackType
		      << ". Stack version: " << ezspVersionDetails.getStackVersionAsString() << "\n";

		// Now request the XNCP version
		this->getXncpInfo();
	}
	else {
		clogI << "EZSP version " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[0]) << " is not supported !" << std::endl;
	}
}

void CLibEzspMain::handleEzspRxMessage_EZSP_GET_XNCP_INFO(const NSSPI::ByteBuffer& i_msg_receive) {
	//clogD << "Got EZSP_GET_XNCP_INFO payload:" << NSSPI::Logger::byteSequenceToString(i_msg_receive) << "\n";

	if (i_msg_receive.size() < 5) {
		clogE << "Wrong size for EZSP_GET_XNCP_INFO message: " << static_cast<unsigned int>(i_msg_receive.size()) << " bytes\n";
	}
	else {
		if (i_msg_receive[0] != EMBER_SUCCESS) {
			clogW << "EZSP_GET_XNCP_INFO failed\n";
		}
		else {
			this->dongle.setFetchedXncpData(dble_u8_to_u16(i_msg_receive[2], i_msg_receive[1]),
			                                dble_u8_to_u16(i_msg_receive[4], i_msg_receive[3]));
		}
	}
	// Now, configure and startup the adapter's embedded stack
	setState(CLibEzspInternal::State::STACK_INIT);
	stackInit();
}

void CLibEzspMain::handleEzspRxMessage_NETWORK_STATE(const NSSPI::ByteBuffer& i_msg_receive)
{
	if (this->getState() != CLibEzspInternal::State::STACK_INIT)
	{
		clogW << "Got EZSP_NETWORK_STATE with value " << static_cast<unsigned int>(i_msg_receive.at(0)) << " while not in STACK_INIT state... assuming stack has been initialized\n";
	}
	clogI << "handleEzspRxMessage_NETWORK_STATE getting EZSP_NETWORK_STATE=" << static_cast<unsigned int>(i_msg_receive.at(0)) << " while CLibEzspInternal::State=" << CLibEzspInternal::getStateAsString(this->getState()) << "\n";
	if (i_msg_receive.at(0) == EMBER_NO_NETWORK) {
		/* No network exists on the dongle */
		clogD << "No pre-existing network on the EZSP adapter\n";
		if (this->resetDot154ChannelAtInit == 0) {
			clogE << "No channel value has been provided and no network is configured in the adapter at init. Cannot continue initialization\n";
			this->setState(CLibEzspInternal::State::INIT_FAILED);
		}
		else {
			/* We create a network on the required channel */
			if (this->getState() == CLibEzspInternal::State::STACK_INIT) {
				clogI << "Creating new network on channel " << static_cast<unsigned int>(this->resetDot154ChannelAtInit) << "\n";
				zb_nwk.formHaNetwork(static_cast<uint8_t>(this->resetDot154ChannelAtInit));
				/* Update our internal state, we are now waiting for a network creation success (EZSP_STACK_STATUS_HANDLER with status==EMBER_NETWORK_UP */
				this->setState(CLibEzspInternal::State::FORM_NWK_IN_PROGRESS);
				this->resetDot154ChannelAtInit = 0; /* Prevent any subsequent network re-creation */
			}
		}
	}
	else
	{
		if ((this->getState() == CLibEzspInternal::State::STACK_INIT) && (this->resetDot154ChannelAtInit != 0))
		{
			clogI << "Zigbee reset requested... Leaving current network\n";
			// leave current network
			zb_nwk.leaveNetwork();
			this->setState(CLibEzspInternal::State::LEAVE_NWK_IN_PROGRESS);
		}
	}
}

void CLibEzspMain::handleEzspRxMessage_EZSP_LAUNCH_STANDALONE_BOOTLOADER(const NSSPI::ByteBuffer& i_msg_receive)
{
    if( this->getState() == CLibEzspInternal::State::SWITCHING_TO_BOOTLOADER_MODE )
    {
        clogD << "Bootloader prompt mode is now going to start. Scheduling selection of the firmware upgrade option.\n";
        this->dongle.setMode(CEzspDongle::Mode::BOOTLOADER_FIRMWARE_UPGRADE);
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
	if (this->getState() != CLibEzspInternal::State::LEAVE_NWK_IN_PROGRESS)
	{
		clogD << "CEZSP_STACK_STATUS_HANDLER status : " << CEzspEnum::EEmberStatusToString(status) << "\n";
		/* Note: we start the sink below only if network is up, but even if this is the case, we will not do it if we have been asked to reset the Zigbee network
		* Indeed, if the Zigbee network needs to be reset, we will first have to leave and re-create a network in the EZSP_NETWORK_STATE case below, and only then
		* will we get called again with EMBER_NETWORK_UP once the Zigbee network has been re-created */
		if ((EMBER_NETWORK_UP == status) && (this->resetDot154ChannelAtInit == 0))
		{
			this->setState(CLibEzspInternal::State::SINK_BUSY);
			/* Create a sink state change callback to find out when the sink is ready */
			/* When the sink becomes ready, then libezsp will also switch to ready state */
			auto clibobs = [this](CGpSink::State& i_state) -> bool {
				clogD << "Underneath sink changed to state: " << std::dec << static_cast<unsigned int>(i_state) << ", current libezsp state: " << static_cast<unsigned int>(this->getState()) << "\n";
				if ((CGpSink::State::SINK_READY == i_state) &&
					(this->getState() == CLibEzspInternal::State::SINK_BUSY)) {
				   this->setState(CLibEzspInternal::State::READY);
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
            if (this->getState() != CLibEzspInternal::State::LEAVE_NWK_IN_PROGRESS)
            {
                clogW << "Got EZSP_LEAVE_NETWORK while not in CLibEzspInternal::State=LEAVE_NWK_IN_PROGRESS\n";
            }
            // Reset our current state to stack init
            this->setState(CLibEzspInternal::State::STACK_INIT);
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
            if (this->getState() != CLibEzspInternal::State::SCANNING)
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
            if (this->getState() != CLibEzspInternal::State::SCANNING)
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
            if (this->getState() != CLibEzspInternal::State::SCANNING)
            {
                clogW << "Got a EZSP_SCAN_COMPLETE_HANDLER message while not in SCANNING state\n";
            }
            else
            {
                clogD << "Scan finished\n";
            }
            this->scanInProgress = false;
            if (this->getState() == CLibEzspInternal::State::SCANNING) {
                this->setState(CLibEzspInternal::State::READY);
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
			clogI << "Unhandled EZSP message: " << NSSPI::Logger::byteSequenceToString(bufDump) << "\n";
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

	uint32_t sourceId = i_gpf.getSourceId();
	if (this->registeredSourceIdsStats.find(sourceId) != this->registeredSourceIdsStats.end()) {
		/* This source ID is registered for stats */
		clogD << "Source ID " << std::hex << std::setw(8) << std::setfill('0') << sourceId << " is known and its report frames will be monitored\n";
		NSEZSP::Stats::SourceIdData& sourceIdStat = this->registeredSourceIdsStats[sourceId];
		if (sourceIdStat.timer == nullptr) {
			sourceIdStat.timer = std::move(this->timerbuilder.create());    /* Allocate a timer for this source ID record if it does not exist yet */
		}
		else {
			sourceIdStat.timer->stop(); /* In case there is already a timer armed on this source ID, cancel it right away */
		}
		uint32_t sourceIdReportTimeout = static_cast<uint32_t>(1000 * NSEZSP::Stats::SourceIdData::REPORTS_AVG_PERIOD * 1.25);
		/* Arm a callback at REPORTS_AVG_PERIOD +25% to take action if the next report for this source ID does not occur in the expected timeframe */
		//clogD << "Arming a timer for source ID " << std::hex << std::setw(8) << std::setfill('0') << sourceId << " for "
		//      << std::dec << std::setw(0) << sourceIdReportTimeout << "ms\n";

		sourceIdStat.timer->start(sourceIdReportTimeout,  /* Note: timer duration is expressed in ms */
								  [this,sourceId,sourceIdReportTimeout](NSSPI::ITimer* triggeringTimer) {
			if (this->registeredSourceIdsStats.find(sourceId) == this->registeredSourceIdsStats.end()) {
				clogW << "Timeout triggered for watching a source ID that is not in our registered list anymore\n";
				return;
			}
			clogW << "Source ID " << std::hex << std::setw(8) << std::setfill('0') << sourceId
			      << " did not send a report frame in a timely manner (first miss detected after "
			      << std::dec << std::setw(0) << sourceIdReportTimeout
			      << "ms). Writing a record about this.\n";
			NSEZSP::Stats::SourceIdData& sourceIdStat = this->registeredSourceIdsStats[sourceId];
			sourceIdStat.nbSuccessiveMisses = 1;
			sourceIdStat.write();
			sourceIdStat.nbSuccessiveRx = 0;
		});
		std::time_t oldTimestamp = sourceIdStat.lastSeenTimeStamp;
		std::time_t now = std::time(nullptr);
		if (oldTimestamp == NSEZSP::Stats::SourceIdData::unknown) {
			if (sourceIdStat.outputFile.is_open()) {
				clogW << "Output file is already opened but no lastSeenTimeStamp is set, this is dodgy\n";
			}
			else {
				std::stringstream outputFilename;
#ifdef USE_RARITAN
				outputFilename << "/flashdisk/";
#else
				outputFilename << "/tmp/";
#endif
				outputFilename << "libezsp-" << std::hex << std::setw(8) << std::setfill('0') << sourceId << ".db";
				clogD << "First time seen source ID " << std::hex << std::setw(8) << std::setfill('0') << sourceId
				      << " at " << now << ". Creating a logfile \"" << outputFilename.str() << "\"\n";

				sourceIdStat.outputFile.open(outputFilename.str(), std::fstream::out | std::fstream::app);
				/* We build a specific marker record below, with all bits of offlineSequenceNo, nbSuccessiveMisses and nbSuccessiveRx set to 1)
				   This allows us to differenciate it with normal data. This marker denotes the absolute timestamp of the beggining of logs */
				sourceIdStat.lastSeenTimeStamp = now;
				sourceIdStat.offlineSequenceNo = -1;
				sourceIdStat.nbSuccessiveMisses = -1;
				sourceIdStat.nbSuccessiveRx = -1;
				/* Append a restart marker record to the file, specifying the timestamp for the first packet */
				sourceIdStat.write();
				sourceIdStat.offlineSequenceNo = 1;
				sourceIdStat.nbSuccessiveMisses = 0;
				sourceIdStat.nbSuccessiveRx = 1; /* We can now count 1 successfull reception */
			}
		}
		else {
			double elapsed = std::difftime(now, oldTimestamp);
			std::stringstream msg;
			msg << "Elapsed since last seen: " << elapsed << "s";
			if (elapsed<0) {
				clogE << msg.str() << "=> negative delta error\n";
			}
			else {
				if (elapsed > (NSEZSP::Stats::SourceIdData::REPORTS_AVG_PERIOD * 1.25)) {    /* Give 25% tolerance on reports period */
					/* If over this tolerance, assume we have missed at least one report */
					uint32_t nbMisses = (elapsed + (0.25 * NSEZSP::Stats::SourceIdData::REPORTS_AVG_PERIOD) / NSEZSP::Stats::SourceIdData::REPORTS_AVG_PERIOD - 1;   /* Compute the number of missed reports */
					sourceIdStat.nbSuccessiveMisses = nbMisses;
					/* The bumber of missed sequences has already been incremented at first miss (timer) */
					sourceIdStat.write();
					clogD << msg.str() << ". Writing record #" << std::dec << std::setw(0) << sourceIdStat.offlineSequenceNo
					      << " for " << nbMisses << " missed report frame(s) after " << sourceIdStat.nbSuccessiveRx
					      << " successfully received report frames, due to no reception during " << elapsed << "s starting from " << sourceIdStat.lastSeenTimeStamp << "\n";
					sourceIdStat.offlineSequenceNo++;   /* Increment the number of missed sequences */
					sourceIdStat.nbSuccessiveMisses = 0;    /* No miss anymore */
					sourceIdStat.nbSuccessiveRx = 1;    /* We can now count the first new successful reception in this sequence (after missed report frames) */
					sourceIdStat.lastSeenTimeStamp = now;
					sourceIdStat.write();
				}
				else {
					/* Received a report within the expected period */
					sourceIdStat.nbSuccessiveMisses = 0;    /* No successive miss */
					sourceIdStat.nbSuccessiveRx++;
					clogD << msg.str() << ". No miss detected (cumulative succesfully received report frames: "
					      << std::dec << std::setw(0) << sourceIdStat.nbSuccessiveRx << ")\n";
					/* We won't write anything to the disk to avoid continuous write... this is the nominal situation and should occur most of the time. We'll only log failures */
				}
			}
		}
		sourceIdStat.lastSeenTimeStamp = now;
	}
	else {
		clogW << "Source ID " << std::hex << std::setw(8) << std::setfill('0') << sourceId << " is unknown and won't be logged\n";
	}

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
