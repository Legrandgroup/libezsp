/**
 * @file lib-ezsp-main.cpp
 */

#include "ezsp/lib-ezsp-main.h"
#include "spi/ILogger.h"
#include "ezsp/ezsp-protocol/get-network-parameters-response.h"  // For CGetNetworkParamtersResponse
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
    STACK_INIT,                         /*<! We are starting up the Zigbee stack in the adapter */
    FORM_NWK_IN_PROGRESS,               /*<! We are currently creating a new Zigbee network */
    LEAVE_NWK_IN_PROGRESS,              /*<! We are currently leaving the Zigbee network we previously joined */
    READY,                              /*<! Library is ready to work and process new command */
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
    exp_ezsp_version(6),
    lib_state(CLibEzspInternalState::UNINITIALIZED),
    obsStateCallback(nullptr),
    dongle(timerbuilder, this),
    zb_messaging(dongle, timerbuilder),
    zb_nwk(dongle, zb_messaging),
    gp_sink(dongle, zb_messaging),
    obsGPFrameRecvCallback(nullptr),
    obsGPSourceIdCallback(nullptr),
    resetDot154ChannelAtInit(requestZbNetworkResetToChannel)
{
    // uart
    if( dongle.open(uartDriver) ) {
        clogI << "CLibEzspMain open success !" << std::endl;
        dongle.registerObserver(this);
        gp_sink.registerObserver(this);
        setState(CLibEzspInternalState::WAIT_DONGLE_READY);  /* Because the dongle observer has been set to ourselves just above, our handleDongleState() method will be called back as soon as the dongle is detected */
    }
}

void CLibEzspMain::registerLibraryStateCallback(FLibStateCallback newObsStateCallback)
{
    this->obsStateCallback = newObsStateCallback;
}

void CLibEzspMain::registerGPFrameRecvCallback(FGpFrameRecvCallback newObsGPFrameRecvCallback)
{
    this->obsGPFrameRecvCallback = newObsGPFrameRecvCallback;
}

void CLibEzspMain::registerGPSourceIdCallback(FGpdSourceIdCallback newObsGPSourceIdCallback)
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
            case CLibEzspInternalState::STACK_INIT:
                obsStateCallback(CLibEzspState::UNINITIALIZED);
                break;
            case CLibEzspInternalState::READY:
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
     */
    setState(CLibEzspInternalState::GETTING_EZSP_VERSION);
    dongle.sendCommand(EEzspCmd::EZSP_VERSION, std::vector<uint8_t>({ezsp_version}));
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
            dongleInit(this->exp_ezsp_version);
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

void CLibEzspMain::setAnswerToGpfChannelRqstPolicy(bool allowed)
{
    this->gp_sink.authorizeAnswerToGpfChannelRqst(allowed);
}

void CLibEzspMain::jumpToBootloader()
{
    this->dongle.sendCommand(EZSP_LAUNCH_STANDALONE_BOOTLOADER, { 0x01 });  /* 0x00 for STANDALONE_BOOTLOADER_NORMAL_MODE */
    this->setState(CLibEzspInternalState::SWITCH_TO_BOOTLOADER_IN_PROGRESS);
    clogE << "Dongle is now in bootloader mode... note: the rest of the procedure is not yet implemented!\n";
    /* Should now receive an EZSP_LAUNCH_STANDALONE_BOOTLOADER in the handleEzspRxMessage handler below, and only then, issue a carriage return to get the bootloader prompt */
}

void CLibEzspMain::handleEzspRxMessage_VERSION(std::vector<uint8_t> i_msg_receive )
{
	// Check if the wanted protocol version, and display stack version
	if( i_msg_receive.at(0) > exp_ezsp_version )
	{
		clogW << "Current EZSP version supported by dongle (" << static_cast<int>(i_msg_receive.at(0)) << ") is higher than our minimum (" << static_cast<int>(exp_ezsp_version) << "). Re-initializing dongle\n";
		exp_ezsp_version = i_msg_receive.at(0);
		dongleInit(exp_ezsp_version);
	}
	else if( i_msg_receive.at(0) == exp_ezsp_version )
	{
		// all is good
		std::stringstream bufDump;

		// protocol
		bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[0]) << ".";

		// type
		bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[1]) << ".";

		// version
		uint16_t l_version = dble_u8_to_u16(i_msg_receive[3], i_msg_receive[2]);
		bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(l_version);

		clogI << "Stack version : " << bufDump.str() << std::endl;

		// configure stack for this application
		setState(CLibEzspInternalState::STACK_INIT);
		stackInit();
	}
	else
	{
		clogI << "EZSP version " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[0]) << " Not supported !" << std::endl;
	}
}

void CLibEzspMain::handleEzspRxMessage_NETWORK_STATE(std::vector<uint8_t> i_msg_receive )
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
			clogD << "Zigbee reset requested... Leaving current network\n";
			// leave current network
			zb_nwk.leaveNetwork();
			this->setState(CLibEzspInternalState::LEAVE_NWK_IN_PROGRESS);
		}
	}
}

void CLibEzspMain::handleEzspRxMessage_STACK_STATUS_HANDLER(std::vector<uint8_t> i_msg_receive )
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
				clogD << "Underneath sink changed to state: " << static_cast<unsigned int>(i_state) << ", current libezsp state: " << static_cast<unsigned int>(this->getState()) << "\n";
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

void CLibEzspMain::handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive )
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
            CGetNetworkParamtersResponse l_rsp(i_msg_receive);
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
            if( this->getState() == CLibEzspInternalState::SWITCH_TO_BOOTLOADER_IN_PROGRESS )
            {
                clogD << "Bootloader prompt mode is going to start now\n";
                this->dongle.setBootloaderMode(true);
            }
            else
            {
                clogE << "Unexpected switch over to bootloader mode. Further commands will fail\n";
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
