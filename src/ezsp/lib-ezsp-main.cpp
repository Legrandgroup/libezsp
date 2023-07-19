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
#include <ezsp/zbmessage/zcl-frame.h>
#include "ezsp/ezsp-protocol/get-network-parameters-response.h"  // For CGetNetworkParametersResponse
#include "ezsp/ezsp-protocol/struct/ember-key-struct.h"  // For CEmberKeyStruct
#include "ezsp/ezsp-protocol/struct/ember-gp-proxy-table-entry-struct.h" // For CEmberGpProxyTableEntryStruct
#include "ezsp/ezsp-protocol/struct/ember-gp-address-struct.h" // For CEmberGpAddressStruct

DEFINE_ENUM(State, CLIBEZSP_INTERNAL_STATE_LIST, NSEZSP::CLibEzspInternal);

using NSEZSP::CLibEzspMain;
using NSEZSP::CLibEzspPublic;
using NSEZSP::CLibEzspInternal;

CLibEzspMain::CLibEzspMain(NSSPI::IUartDriverHandle uartHandle, const NSSPI::TimerBuilder& timerbuilder, unsigned int requestZbNetworkResetToChannel) :
	uartHandle(uartHandle),
	timerbuilder(timerbuilder),
	exp_ezsp_min_version(6),    /* Expect EZSP version 6 minimum */
	exp_ezsp_max_version(9),    /* Expect EZSP version 8 maximum */
	exp_stack_type(2),  /* Expect a stack type=2 (mesh) */
	xncpManufacturerId(0),  /* 0 means unknown (yet) */
	xncpVersionNumber(0),  /* 0 means unknown (yet) */
	dongleEUI64(),
	lib_state(CLibEzspInternal::State::UNINITIALIZED),
	obsStateCallback(nullptr),
	dongle(timerbuilder, this),
	zb_messaging(dongle, timerbuilder),
	zb_nwk(dongle, zb_messaging),
	gp_sink(dongle, zb_messaging),
	obsGPFrameRecvCallback(nullptr),
	obsGPSourceIdCallback(nullptr),
	obsZclFrameRecvCallback(nullptr),
	obsBindingTableRecvCallback(nullptr),
	obsTrustCenterJoinHandlerCallback(nullptr),
	obsGpProxyTableEntryJoinHandlerCallback(nullptr),
	obsZdpDeviceAnnounceRecvCallback(nullptr),
	obsZdpActiveEpRecvCallback(nullptr),
	obsZdpSimpleDescRecvCallback(nullptr),
	obsDongleEUI64RecvCallback(nullptr),
	energyScanCallback(nullptr),
	networkKeyCallback(nullptr),
	leavePreviousNetworkAtInit(requestZbNetworkResetToChannel != 0),
	resetDot154ChannelAtInit(requestZbNetworkResetToChannel),
	scanInProgress(false),
	lastChannelToEnergyScan(),
	lastChannelToZigbeeNetworkScan() {
}

void CLibEzspMain::start() {
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

void CLibEzspMain::forceFirmwareUpgradeOnInitTimeout() {
	this->dongle.forceFirmwareUpgradeOnInitTimeout();
}

void CLibEzspMain::registerLibraryStateCallback(FLibStateCallback newObsStateCallback) {
	this->obsStateCallback = newObsStateCallback;
}

void CLibEzspMain::registerZclFrameRecvCallback(FZclFrameRecvCallback newObsZclFrameRecvCallback) {
	this->obsZclFrameRecvCallback = newObsZclFrameRecvCallback;
}

void CLibEzspMain::registerBindingTableRecvCallback(FBindingTableRecvCallback newObsBindingTableRecvCallback) {
	this->obsBindingTableRecvCallback = newObsBindingTableRecvCallback;
}

void CLibEzspMain::registerTrustCenterJoinHandlerCallback(FTrustCenterJoinHandlerCallBack newObsTrustCenterJoinHandlerCallback) {
	this->obsTrustCenterJoinHandlerCallback = newObsTrustCenterJoinHandlerCallback;
}

void CLibEzspMain::registerGpProxyTableEntryJoinHandlerCallback(FGpProxyTableEntryHandlerCallBack newObsGpProxyTableEntryJoinHandlerCallback) {
	this->obsGpProxyTableEntryJoinHandlerCallback = newObsGpProxyTableEntryJoinHandlerCallback;
}

void CLibEzspMain::registerZdpDeviceAnnounceRecvCallback(FZdpDeviceAnnounceCallBack newObsZdpDeviceAnnounceRecvCallback) {
	this->obsZdpDeviceAnnounceRecvCallback = newObsZdpDeviceAnnounceRecvCallback;
}

void CLibEzspMain::registerZdpActiveEpRecvCallback(FZdpActiveEpCallBack newObsZdpActiveEpRecvCallback) {
	this->obsZdpActiveEpRecvCallback = newObsZdpActiveEpRecvCallback;
}

void CLibEzspMain::registerDongleEUI64RecvCallback(FDongleEUI64CallBack newObsDongleEUI64RecvCallback){
	this->obsDongleEUI64RecvCallback = newObsDongleEUI64RecvCallback;
}

void CLibEzspMain::registerZdpSimpleDescRecvCallback(FZdpSimpleDescCallBack newObsZdpSimpleDescRecvCallback){
	this->obsZdpSimpleDescRecvCallback = newObsZdpSimpleDescRecvCallback;
}

void CLibEzspMain::registerGPFrameRecvCallback(FGpFrameRecvCallback newObsGPFrameRecvCallback) {
	this->obsGPFrameRecvCallback = newObsGPFrameRecvCallback;
}

void CLibEzspMain::registerGPSourceIdCallback(FGpSourceIdCallback newObsGPSourceIdCallback) {
	this->obsGPSourceIdCallback = newObsGPSourceIdCallback;
}

void CLibEzspMain::setState(CLibEzspInternal::State i_new_state) {
	CLibEzspInternal::State l_old_state = this->lib_state;
	this->lib_state = i_new_state;
	clogD << "CLibEzspMain state changing from " << NSEZSP::CLibEzspInternal::getStateAsString(l_old_state) << " to " << NSEZSP::CLibEzspInternal::getStateAsString(i_new_state) << "\n";
	if (nullptr != obsStateCallback) {
		switch (i_new_state) {
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
		case CLibEzspInternal::State::TERMINATING:
			obsStateCallback(CLibEzspPublic::State::TERMINATING);
			break;
		default:
			clogE << "Internal state can not be translated to public state\n";
		}
	}
}

CLibEzspInternal::State CLibEzspMain::getState() const {
	return this->lib_state;
}

void CLibEzspMain::dongleInit(uint8_t ezsp_version) {
	/* First request stack protocol version using the related EZSP command
	 * Note that we really need to send this specific command just after a reset because until we do, the dongle will refuse most other commands anyway
	/* Response to this should be the reception of an EZSP_VERSION in the handleEzspRxMessage() handler below
	 */
	setState(CLibEzspInternal::State::GETTING_EZSP_VERSION);
	dongle.sendCommand(EEzspCmd::EZSP_VERSION, NSSPI::ByteBuffer({ ezsp_version }));
}

void CLibEzspMain::getXncpInfo() {
	setState(CLibEzspInternal::State::GETTING_XNCP_INFO);
	dongle.sendCommand(EEzspCmd::EZSP_GET_XNCP_INFO);
}

void CLibEzspMain::stackInit() {
	std::vector<SEzspConfig> l_config;

	l_config.push_back({.id = EZSP_CONFIG_NEIGHBOR_TABLE_SIZE,
	                    .value = 16
	                   });
	l_config.push_back({.id = EZSP_CONFIG_APS_UNICAST_MESSAGE_COUNT,
	                    .value = 10
	                   });
	l_config.push_back({.id = EZSP_CONFIG_BINDING_TABLE_SIZE,
	                    .value = 20
	                   });
	l_config.push_back({.id = EZSP_CONFIG_ADDRESS_TABLE_SIZE,
	                    .value = 16
	                   });
	l_config.push_back({.id = EZSP_CONFIG_MULTICAST_TABLE_SIZE,
	                    .value = 16
	                   });
	l_config.push_back({.id = EZSP_CONFIG_ROUTE_TABLE_SIZE,
	                    .value = 16
	                   });
	l_config.push_back({.id = EZSP_CONFIG_DISCOVERY_TABLE_SIZE,
	                    .value = 8
	                   });
	l_config.push_back({.id = EZSP_CONFIG_STACK_PROFILE,
	                    .value = 2
	                   });
	l_config.push_back({.id = EZSP_CONFIG_SECURITY_LEVEL,
	                    .value = 5
	                   });
	l_config.push_back({.id = EZSP_CONFIG_MAX_HOPS,
	                    .value = 30
	                   });
	l_config.push_back({.id = EZSP_CONFIG_MAX_END_DEVICE_CHILDREN,
	                    .value = 32
	                   }); // define number of sleepy end device directly attached to dongle
	l_config.push_back({.id = EZSP_CONFIG_INDIRECT_TRANSMISSION_TIMEOUT,
	                    .value = 7680
	                   });
	l_config.push_back({.id = EZSP_CONFIG_END_DEVICE_POLL_TIMEOUT,
	                    .value = 8
	                   });
	/*l_config.push_back({.id = EZSP_CONFIG_MOBILE_NODE_POLL_TIMEOUT,
	                    .value = 20
	                   });
	l_config.push_back({.id = EZSP_CONFIG_RESERVED_MOBILE_CHILD_ENTRIES,
	                    .value = 0
	                   });*/
	l_config.push_back({.id = EZSP_CONFIG_TX_POWER_MODE,
	                    .value = 0
	                   });
	l_config.push_back({.id = EZSP_CONFIG_DISABLE_RELAY,
	                    .value = 0
	                   });
	l_config.push_back({.id = EZSP_CONFIG_TRUST_CENTER_ADDRESS_CACHE_SIZE,
	                    .value = 2
	                   });
	l_config.push_back({.id = EZSP_CONFIG_SOURCE_ROUTE_TABLE_SIZE,
	                    .value = 8
	                   });
	/*l_config.push_back({.id = EZSP_CONFIG_END_DEVICE_POLL_TIMEOUT_SHIFT,
	                    .value = 6
	                   });*/
	l_config.push_back({.id = EZSP_CONFIG_FRAGMENT_WINDOW_SIZE,
	                    .value = 1
	                   });
	l_config.push_back({.id = EZSP_CONFIG_FRAGMENT_DELAY_MS,
	                    .value = 0
	                   });
	l_config.push_back({.id = EZSP_CONFIG_KEY_TABLE_SIZE,
	                    .value = 12
	                   });
	l_config.push_back({.id = EZSP_CONFIG_APS_ACK_TIMEOUT,
	                    .value = (50*30)+100
	                   });
	l_config.push_back({.id = EZSP_CONFIG_BEACON_JITTER_DURATION,
	                    .value = 3
	                   });
	l_config.push_back({.id = EZSP_CONFIG_END_DEVICE_BIND_TIMEOUT,
	                    .value = 60
	                   });
	l_config.push_back({.id = EZSP_CONFIG_PAN_ID_CONFLICT_REPORT_THRESHOLD,
	                    .value = 2
	                   });
	l_config.push_back({.id = EZSP_CONFIG_REQUEST_KEY_TIMEOUT,
	                    .value = 0
	                   });
	// l_config.push_back({.id = EZSP_CONFIG_CERTIFICATE_TABLE_SIZE,
	//                     .value = 0
	//                    });
	l_config.push_back({.id = EZSP_CONFIG_APPLICATION_ZDO_FLAGS,
	                    .value = 3
	                   });
	l_config.push_back({.id = EZSP_CONFIG_BROADCAST_TABLE_SIZE,
	                    .value = 15
	                   });
	l_config.push_back({.id = EZSP_CONFIG_MAC_FILTER_TABLE_SIZE,
	                    .value = 0
	                   });
	l_config.push_back({.id = EZSP_CONFIG_SUPPORTED_NETWORKS,
	                    .value = 1
	                   });
	l_config.push_back({.id = EZSP_CONFIG_SEND_MULTICASTS_TO_SLEEPY_ADDRESS,
	                    .value = 0
	                   });
	l_config.push_back({.id = EZSP_CONFIG_ZLL_GROUP_ADDRESSES,
	                    .value = 1
	                   });
	l_config.push_back({.id = EZSP_CONFIG_ZLL_RSSI_THRESHOLD,
	                    .value = 128
	                   });
	l_config.push_back({.id = EZSP_CONFIG_MTORR_FLOW_CONTROL,
	                    .value = 1
	                   });
	l_config.push_back({.id = EZSP_CONFIG_RETRY_QUEUE_SIZE,
	                    .value = 16
	                   });
	l_config.push_back({.id = EZSP_CONFIG_NEW_BROADCAST_ENTRY_THRESHOLD,
	                    .value = 9
	                   });
	l_config.push_back({.id = EZSP_CONFIG_TRANSIENT_KEY_TIMEOUT_S,
	                    .value = 300
	                   });
	l_config.push_back({.id = EZSP_CONFIG_BROADCAST_MIN_ACKS_NEEDED,
	                    .value = 255
	                   });
	l_config.push_back({.id = EZSP_CONFIG_TC_REJOINS_USING_WELL_KNOWN_KEY_TIMEOUT_S,
	                    .value = 300
	                   });
	l_config.push_back({.id = EZSP_CONFIG_PACKET_BUFFER_COUNT,
	                    .value = 75
	                   }); // use all remain memory for in/out radio packets

	std::vector<SEzspPolicy> l_policy;
	l_policy.push_back({.id = EZSP_TRUST_CENTER_POLICY,
	                    .decision = EZSP_ALLOW_PRECONFIGURED_KEY_JOINS
	                   });
	l_policy.push_back({.id = EZSP_MESSAGE_CONTENTS_IN_CALLBACK_POLICY,
	                    .decision = EZSP_MESSAGE_TAG_ONLY_IN_CALLBACK
	                   });
	l_policy.push_back({.id = EZSP_BINDING_MODIFICATION_POLICY,
	                    .decision = EZSP_CHECK_BINDING_MODIFICATIONS_ARE_VALID_ENDPOINT_CLUSTERS
	                   });
	l_policy.push_back({.id = EZSP_POLL_HANDLER_POLICY,
	                    .decision = EZSP_POLL_HANDLER_IGNORE
	                   });

	zb_nwk.stackInit(l_config, l_policy);
}

/**
 * Oberver handlers
 */
void CLibEzspMain::handleDongleState( EDongleState i_state ) {
	// clogI << __func__ << "() => dongleState : " << i_state << "\n";

	if( DONGLE_READY == i_state ) {
		if( CLibEzspInternal::State::WAIT_DONGLE_READY == getState() ) {
			dongleInit(this->exp_ezsp_min_version);
		}
	}
	else if( DONGLE_REMOVE == i_state ) {
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

bool CLibEzspMain::clearAllGPDevices() {
	if (this->getState() != CLibEzspInternal::State::READY) {
		return false;
	}
	this->setState(CLibEzspInternal::State::SINK_BUSY);
	bool success = this->gp_sink.clearAllGpds();
	if (!success) {
		return false; /* Probably sink is not ready */
	}
	else {
		return true;
	}
}

bool CLibEzspMain::removeGPDevices(const std::vector<uint32_t>& sourceIdList) {
	if (this->getState() != CLibEzspInternal::State::READY) {
		return false;
	}
	this->setState(CLibEzspInternal::State::SINK_BUSY);
	if (!this->gp_sink.removeGpds(sourceIdList)) {
		return false; /* Probably sink is not ready */
	}

	return true;
}

bool CLibEzspMain::addGPDevices(const std::vector<CGpDevice> &gpDevicesList) {
	if (this->getState() != CLibEzspInternal::State::READY) {
		return false;
	}
	this->setState(CLibEzspInternal::State::SINK_BUSY);
	if (!this->gp_sink.registerGpds(gpDevicesList)) {
		return false; /* Probably sink is not ready */
	}

	return true;
}

bool CLibEzspMain::getEUI64(){
	if (this->getState() != CLibEzspInternal::State::READY) {
		return false;
	}
	dongle.sendCommand(EZSP_GET_EUI64);
	return true;
}

bool CLibEzspMain::getGPProxyTableEntry(const int index) {
	if (this->getState() != CLibEzspInternal::State::READY) {
		return false;
	}
	NSSPI::ByteBuffer payload;
	payload.push_back(index);
	dongle.sendCommand(EZSP_GP_PROXY_TABLE_GET_ENTRY, payload);
	return true;
}

bool CLibEzspMain::openCommissioningSession() {
	if (this->getState() != CLibEzspInternal::State::READY) {
		return false;
	}
	this->gp_sink.openCommissioningSession();
	return true;
}

bool CLibEzspMain::closeCommissioningSession() {
	if (this->getState() != CLibEzspInternal::State::READY) {
		return false;
	}
	gp_sink.closeCommissioningSession();
	return true;
}

void CLibEzspMain::setAnswerToGpfChannelRqstPolicy(bool allowed) {
	this->gp_sink.authorizeAnswerToGpfChannelRqst(allowed);
}

void CLibEzspMain::setFirmwareUpgradeMode() {
	this->dongle.sendCommand(EZSP_LAUNCH_STANDALONE_BOOTLOADER, { 0x01 });  /* 0x00 for STANDALONE_BOOTLOADER_NORMAL_MODE */
	this->setState(CLibEzspInternal::State::SWITCHING_TO_BOOTLOADER_MODE);
	/* We should now receive an EZSP_LAUNCH_STANDALONE_BOOTLOADER in the handleEzspRxMessage() handler below, and only then, issue a carriage return to get the bootloader prompt */
}

bool CLibEzspMain::startEnergyScan(FEnergyScanCallback energyScanCallback, uint8_t duration, uint32_t requestedChannelMask) {
	if (this->getState() == CLibEzspInternal::State::SCANNING || this->scanInProgress) {
		clogE << "Ignoring request for energy scan because we're still waiting for a previous scan to finish\n";
		return false;
	}
	NSSPI::ByteBuffer l_payload = { EZSP_ENERGY_SCAN };
	uint32_t channelMask = 0x07FFF800; // Default range from channel 11 to 26 (inclusive)
	if (requestedChannelMask != 0) {
		channelMask = requestedChannelMask;
	}
	l_payload.push_back(u32_get_byte0(channelMask));
	l_payload.push_back(u32_get_byte1(channelMask));
	l_payload.push_back(u32_get_byte2(channelMask));
	l_payload.push_back(u32_get_byte3(channelMask));
	l_payload.push_back(duration);
	this->lastChannelToEnergyScan.clear();
	this->setState(CLibEzspInternal::State::SCANNING);
	this->energyScanCallback = energyScanCallback;
	this->activeScanCallback = nullptr;	/* Discard any conflicting active scan callback */
	this->dongle.sendCommand(EZSP_START_SCAN, l_payload);
	return true;
}

bool CLibEzspMain::startActiveScan(FActiveScanCallback activeScanCallback, uint8_t duration, uint32_t requestedChannelMask) {
	if (this->getState() == CLibEzspInternal::State::SCANNING || this->scanInProgress) {
		clogE << "Ignoring request for energy scan because we're still waiting for a previous scan to finish\n";
		return false;
	}
	NSSPI::ByteBuffer l_payload = { EZSP_ACTIVE_SCAN };
	uint32_t channelMask = 0x07FFF800; // Default range from channel 11 to 26 (inclusive)
	if (requestedChannelMask != 0) {
		channelMask = requestedChannelMask;
	}
	l_payload.push_back(u32_get_byte0(channelMask));
	l_payload.push_back(u32_get_byte1(channelMask));
	l_payload.push_back(u32_get_byte2(channelMask));
	l_payload.push_back(u32_get_byte3(channelMask));
	l_payload.push_back(duration);
	this->lastChannelToZigbeeNetworkScan.clear();
	this->setState(CLibEzspInternal::State::SCANNING);
	this->activeScanCallback = activeScanCallback;
	this->energyScanCallback = nullptr;	/* Discard any conflicting energy scan callback */
	this->dongle.sendCommand(EZSP_START_SCAN, l_payload);
	return true;
}

bool CLibEzspMain::getNetworkKey(FNetworkKeyCallback networkKeyCallback) {
	this->networkKeyCallback = networkKeyCallback;
	dongle.sendCommand(EEzspCmd::EZSP_GET_KEY, NSSPI::ByteBuffer({ EMBER_CURRENT_NETWORK_KEY }));
	return true;
}

bool CLibEzspMain::setChannel(uint8_t channel) {
	if (this->getState() != CLibEzspInternal::State::READY || this->scanInProgress) {
		return false;
	}
	this->dongle.sendCommand(EZSP_SET_RADIO_CHANNEL, { channel });
	return true;
}

bool CLibEzspMain::joinNetwork(NSEZSP::CEmberNetworkParameters& nwkParams) {
	if (this->getState() != CLibEzspInternal::State::READY || this->scanInProgress) {
		return false;
	}
	this->zb_nwk.joinNetwork(nwkParams);
	return true;
}

bool CLibEzspMain::createNetwork(uint8_t channel){
	if (this->getState() != CLibEzspInternal::State::READY || this->scanInProgress) {
		return false;
	}
	this->resetDot154ChannelAtInit = channel;
	this->zb_nwk.leaveNetwork();
	return true;
}

bool CLibEzspMain::openNetwork(uint8_t i_timeout){
	if (this->getState() != CLibEzspInternal::State::READY || this->scanInProgress) {
		return false;
	}
	this->zb_nwk.openNetwork(i_timeout);
	return true;
}

bool CLibEzspMain::closeNetwork(){
	if (this->getState() != CLibEzspInternal::State::READY || this->scanInProgress) {
		return false;
	}
	this->zb_nwk.closeNetwork();
	return true;
}

bool CLibEzspMain::SendZDOCommand(EmberNodeId i_node_id, uint16_t i_cmd_id, const NSSPI::ByteBuffer& payload) {
	if (this->getState() != CLibEzspInternal::State::READY || this->scanInProgress) {
		return false;
	}
	this->zb_messaging.SendZDOCommand(i_node_id, i_cmd_id, payload);
	return true;
}

bool CLibEzspMain::SendZCLCommand(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint8_t i_cmd_id,
								  const NSEZSP::EZCLFrameCtrlDirection i_direction, const NSSPI::ByteBuffer& i_payload,
								  const uint16_t i_node_id, const uint8_t i_transaction_number,
								  const uint16_t i_grp_id, const uint16_t i_manufacturer_code) {
	if (this->getState() != CLibEzspInternal::State::READY || this->scanInProgress) {
		return false;
	}
	this->zb_messaging.SendZCLCommand(i_endpoint, i_cluster_id, i_cmd_id, i_direction, i_payload, i_node_id, i_transaction_number, i_grp_id, i_manufacturer_code);
	return true;
}

bool CLibEzspMain::DiscoverAttributes(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint16_t i_start_attribute_identifier,
									 const uint8_t i_maximum_attribute_identifier, const EZCLFrameCtrlDirection i_direction, const uint16_t i_node_id,
									 const uint8_t i_transaction_number, const uint16_t i_grp_id, const uint16_t i_manufacturer_code){
    if (this->getState() != CLibEzspInternal::State::READY || this->scanInProgress) {
		return false;
	}
	this->zb_messaging.DiscoverAttributes(i_endpoint, i_cluster_id, i_start_attribute_identifier, i_maximum_attribute_identifier, i_direction, i_node_id, i_transaction_number, i_grp_id, i_manufacturer_code);
	return true;
}

bool CLibEzspMain::ReadAttribute(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint16_t i_attribute_id,
				   				 const EZCLFrameCtrlDirection i_direction, const uint16_t i_node_id,
				   				 const uint8_t i_transaction_number, const uint16_t i_grp_id, const uint16_t i_manufacturer_code){
	if (this->getState() != CLibEzspInternal::State::READY || this->scanInProgress) {
		return false;
	}
	this->zb_messaging.ReadAttribute(i_endpoint, i_cluster_id, i_attribute_id, i_direction, i_node_id, i_transaction_number, i_grp_id, i_manufacturer_code);
	return true;
}

bool CLibEzspMain::WriteAttribute(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint16_t i_attribute_id,
								  const EZCLFrameCtrlDirection i_direction, const uint8_t i_datatype, const NSSPI::ByteBuffer& i_data,
								  const uint16_t i_node_id, const uint8_t i_transaction_number,
								  const uint16_t i_grp_id, const uint16_t i_manufacturer_code) {
	if (this->getState() != CLibEzspInternal::State::READY || this->scanInProgress) {
		return false;
	}
	this->zb_messaging.WriteAttribute(i_endpoint, i_cluster_id, i_attribute_id, i_direction, i_datatype, i_data, i_node_id, i_transaction_number, i_grp_id, i_manufacturer_code);
	return true;
}

bool CLibEzspMain::ConfigureReporting(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint16_t i_attribute_id,
									  const EZCLFrameCtrlDirection i_direction, const uint8_t i_datatype, const uint16_t i_min,
									  const uint16_t i_max, const uint16_t i_reportable, const uint16_t i_node_id,
									  const uint8_t i_transaction_number, const uint16_t i_grp_id, const uint16_t i_manufacturer_code) {
	if (this->getState() != CLibEzspInternal::State::READY || this->scanInProgress) {
		return false;
	}
	this->zb_messaging.ConfigureReporting(i_endpoint, i_cluster_id, i_attribute_id, i_direction, i_datatype, i_min, i_max, i_reportable, i_node_id, i_transaction_number, i_grp_id, i_manufacturer_code);
	return true;
}

void CLibEzspMain::handleFirmwareXModemXfr() {
	this->setState(CLibEzspInternal::State::IN_XMODEM_XFR);
	clogW << "EZSP adapter is now ready to receive a firmware image (.gbl) via X-modem\n";
}

void CLibEzspMain::handleEzspRxMessage_VERSION(const NSSPI::ByteBuffer& i_msg_receive) {
	std::stringstream bufDump;
	bool truncatedVersion = false;  /* Flag set to true when receiving a truncated EZSP_VERSION payload */
	bool acceptableVersion = false; /* Flag set to true when the version received from the EZSP adapter is acceptable for us */
	clogD << "Got EZSP_VERSION payload:" << i_msg_receive << "\n";
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
	//clogD << "Got EZSP_GET_XNCP_INFO payload:" << i_msg_receive << "\n";

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

void CLibEzspMain::handleEzspRxMessage_NETWORK_STATE(const NSSPI::ByteBuffer& i_msg_receive) {
	//clogI << "handleEzspRxMessage_NETWORK_STATE getting EZSP_NETWORK_STATE=" << static_cast<unsigned int>(i_msg_receive.at(0)) << " while CLibEzspInternal::State=" << CLibEzspInternal::getStateAsString(this->getState()) << "\n";
	if (this->getState() == CLibEzspInternal::State::FORM_NWK_IN_PROGRESS && i_msg_receive.at(0) == EMBER_NO_NETWORK) {
		clogD << "Adapter is not yet in a network (while forming is in progress)... still waiting for EMBER_NETWORK_UP\n";
		return;
	}
	if (this->getState() != CLibEzspInternal::State::STACK_INIT
	    && this->getState() != CLibEzspInternal::State::FORM_NWK_IN_PROGRESS
	    && this->getState() != CLibEzspInternal::State::LEAVE_NWK_IN_PROGRESS) {
		clogW << "Unexpectedly got EZSP_NETWORK_STATE with value " << static_cast<unsigned int>(i_msg_receive.at(0)) << " while not in STACK_INIT or FORM_NWK_IN_PROGRESS or LEAVE_NWK_IN_PROGRESS state... assuming stack has been initialized. Ignoring...\n";
	}
	if (i_msg_receive.at(0) == EMBER_NO_NETWORK) {
		/* No network exists on the dongle */
		clogD << "No pre-existing network on the EZSP adapter\n";
		if (this->resetDot154ChannelAtInit == static_cast<unsigned int>(-1)) {
			/* We are outside of a network and we were actually asked to leave... just terminate now */
			clogI << "Adapter is not part of any network as requested. Terminating\n";
			this->setState(CLibEzspInternal::State::TERMINATING);
			return;
		}
		if (this->resetDot154ChannelAtInit == 0) {
			clogE << "No channel value has been provided and the adapter has not currently joined any network. Cannot continue initialization\n";
			this->setState(CLibEzspInternal::State::INIT_FAILED);
			return;
		}
		/* We create a network on the required channel */
		if (this->getState() == CLibEzspInternal::State::STACK_INIT) {
			clogI << "Creating new network on channel " << static_cast<unsigned int>(this->resetDot154ChannelAtInit) << "\n";
			zb_nwk.formHaNetwork(static_cast<uint8_t>(this->resetDot154ChannelAtInit));
			/* Update our internal state, we are now waiting for a network creation success (EZSP_STACK_STATUS_HANDLER with status==EMBER_NETWORK_UP */
			this->setState(CLibEzspInternal::State::FORM_NWK_IN_PROGRESS);
			this->resetDot154ChannelAtInit = 0; /* Prevent any subsequent network re-creation */
		}
	}
	else {
		if ((this->getState() == CLibEzspInternal::State::STACK_INIT) && (this->resetDot154ChannelAtInit != 0)) {
			clogI << "Zigbee reset requested... Leaving current network\n";
			// leave current network
			zb_nwk.leaveNetwork();
			this->setState(CLibEzspInternal::State::LEAVE_NWK_IN_PROGRESS);
		}
	}
}

void CLibEzspMain::handleEzspRxMessage_EZSP_LAUNCH_STANDALONE_BOOTLOADER(const NSSPI::ByteBuffer& i_msg_receive) {
	if( this->getState() == CLibEzspInternal::State::SWITCHING_TO_BOOTLOADER_MODE ) {
		clogD << "Bootloader prompt mode is now going to start. Scheduling selection of the firmware upgrade option.\n";
		this->dongle.setMode(CEzspDongle::Mode::BOOTLOADER_FIRMWARE_UPGRADE);
	}
	else {
		clogE << "Unexpected switch over to bootloader mode. Further commands will fail\n";
	}
}

void CLibEzspMain::handleEzspRxMessage_STACK_STATUS_HANDLER(const NSSPI::ByteBuffer& i_msg_receive) {
	EEmberStatus status = static_cast<EEmberStatus>(i_msg_receive.at(0));
	/* We handle EZSP_STACK_STATUS_HANDLER only if we are not currently leaving network.
	 * This is because EZSP adapter send spurious EMBER_NETWORK_UP or EMBER_NOT_JOINED while leaving the network.
	 * We will ignore all these until we get a EZSP_LEAVE_NETWORK message (see handler below)
	 */
	if (this->getState() != CLibEzspInternal::State::LEAVE_NWK_IN_PROGRESS) {
		clogD << "CEZSP_STACK_STATUS_HANDLER status : " << CEzspEnum::EEmberStatusToString(status) << "\n";
		/* Note: we start the sink below only if network is up, but even if this is the case, we will not do it if we have been asked to reset the Zigbee network
		* Indeed, if the Zigbee network needs to be reset, we will first have to leave and re-create a network in the EZSP_NETWORK_STATE case below, and only then
		* will we get called again with EMBER_NETWORK_UP once the Zigbee network has been re-created */
		if ((EMBER_NETWORK_UP == status) && (this->resetDot154ChannelAtInit == 0)) {
			/* Retrieve dongle's EUI64 MAC address */
			dongle.sendCommand(EZSP_GET_EUI64);

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
		else {
			if (155 == status) {	/* We receive status 155 just after having done a channel change */
				clogD << "Got channel change feedback. Retrieving new network parameters\n";
				/* In such case, we must retrieve again the network parameters, amongst other, to update the GP sink with the correct new channel */
				dongle.sendCommand(EZSP_GET_NETWORK_PARAMETERS);
			}
			else {
				clogD << "Call EZSP_NETWORK_STATE\n";
				dongle.sendCommand(EZSP_NETWORK_STATE);
			}
		}
	}
	else {
		clogD << "Ignoring CEZSP_STACK_STATUS_HANDLER status (while in network leave state): " << CEzspEnum::EEmberStatusToString(status) << "\n";
	}
}

void CLibEzspMain::handleEzspRxMessage(EEzspCmd i_cmd, NSSPI::ByteBuffer i_msg_receive) {
	clogD << "CLibEzspMain::handleEzspRxMessage " << CEzspEnum::EEzspCmdToString(i_cmd);
	if (i_msg_receive.size()>0) {
		clogD << " with payload " << i_msg_receive;
	}
	clogD << "\n";

	switch( i_cmd ) {
	case EZSP_STACK_STATUS_HANDLER: {
		handleEzspRxMessage_STACK_STATUS_HANDLER(i_msg_receive);
	}
	break;
	case EZSP_GET_NETWORK_PARAMETERS: {
		CGetNetworkParametersResponse l_rsp(i_msg_receive);
		clogI << l_rsp.String() << std::endl;
	}
	break;
	case EZSP_GET_KEY: {
		EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));
		i_msg_receive.erase(i_msg_receive.begin());
		CEmberKeyStruct l_rsp(i_msg_receive);
		clogI << "EZSP_GET_KEY status : " << CEzspEnum::EEmberStatusToString(l_status) << ", " << l_rsp.String() << std::endl;
		if (this->networkKeyCallback) {
			this->networkKeyCallback(l_status, l_rsp.getKey());
			this->networkKeyCallback = nullptr;  /* Disable callback */
		}
	}
	break;
	case EZSP_GET_EUI64:
	{

		dongleEUI64.clear();

		for( uint8_t loop=0; loop<EMBER_EUI64_BYTE_SIZE; loop++ ) {
			dongleEUI64.push_back(i_msg_receive.at(loop));
		}

		if( nullptr != obsDongleEUI64RecvCallback ) {
			obsDongleEUI64RecvCallback(dongleEUI64);
		}
	}
	break;
	case EEzspCmd::EZSP_VERSION: {
		handleEzspRxMessage_VERSION(i_msg_receive);
	}
	break;
	case EZSP_GET_XNCP_INFO: {
		handleEzspRxMessage_EZSP_GET_XNCP_INFO(i_msg_receive);
	}
	break;
	case EZSP_NETWORK_STATE: {
		handleEzspRxMessage_NETWORK_STATE(i_msg_receive);
	}
	break;
	case EZSP_LEAVE_NETWORK: {
		if (this->getState() != CLibEzspInternal::State::LEAVE_NWK_IN_PROGRESS) {
			clogW << "Got EZSP_LEAVE_NETWORK while not in CLibEzspInternal::State=LEAVE_NWK_IN_PROGRESS\n";
		}
		// Reset our current state to stack init
		this->setState(CLibEzspInternal::State::STACK_INIT);
	}
	break;
	case EZSP_LAUNCH_STANDALONE_BOOTLOADER: {
		handleEzspRxMessage_EZSP_LAUNCH_STANDALONE_BOOTLOADER(i_msg_receive);
	}
	break;
	case EZSP_START_SCAN: {
		if (this->getState() != CLibEzspInternal::State::SCANNING) {
			clogW << "Got a EZSP_START_SCAN message while not in SCANNING state\n";
		}
		this->scanInProgress = (EMBER_SUCCESS == i_msg_receive.at(0));
		if (this->scanInProgress) {
			clogD << "Scan succesfully started\n";
		}
	}
	break;
	case EZSP_ENERGY_SCAN_RESULT_HANDLER: {
		if (this->getState() != CLibEzspInternal::State::SCANNING) {
			clogW << "Got a EZSP_ENERGY_SCAN_RESULT_HANDLER message while not in SCANNING state\n";
		}
		uint8_t channel = i_msg_receive.at(0);
		int8_t rssi = static_cast<int8_t>(i_msg_receive.at(1));
		this->lastChannelToEnergyScan.emplace(channel, rssi);
		clogD << "EZSP_ENERGY_SCAN_RESULT_HANDLER: channel: " << std::dec << static_cast<unsigned int>(channel) << " rssi: " << static_cast<int>(rssi) << " dBm\n";
	}
	break;
	case EZSP_NETWORK_FOUND_HANDLER: {
		if (this->getState() != CLibEzspInternal::State::SCANNING) {
			clogW << "Got a EZSP_NETWORK_FOUND_HANDLER message while not in SCANNING state\n";
		}
		NSEZSP::CEmberZigbeeNetwork networkFound(i_msg_receive);
		uint8_t lastHopLqi = static_cast<uint8_t>(i_msg_receive.at(14));
		int8_t lastHopRssi = static_cast<int8_t>(i_msg_receive.at(15));
		uint8_t channel = networkFound.getChannel();
		NSEZSP::ZigbeeNetworkScanResult networkScan(networkFound, lastHopLqi, lastHopRssi);	/* Create a new network scan containing this scan result */

		std::vector<NSEZSP::ZigbeeNetworkScanResult>& zigbeeActiveNetworkScanResults = this->lastChannelToZigbeeNetworkScan[channel];
		zigbeeActiveNetworkScanResults.push_back(networkScan);	/* Add to the set for this channel, an entry with the new network scan inside */
		clogW << "EZSP_NETWORK_FOUND_HANDLER: New network found: " << networkFound << "\n";
	}
	break;
	case EZSP_SCAN_COMPLETE_HANDLER: {
		if (this->getState() != CLibEzspInternal::State::SCANNING) {
			clogW << "Got a EZSP_SCAN_COMPLETE_HANDLER message while not in SCANNING state\n";
		}
		else {
			clogD << "Scan finished\n";
		}
		this->scanInProgress = false;
		if (this->getState() == CLibEzspInternal::State::SCANNING) {
			this->setState(CLibEzspInternal::State::READY);
			if (this->energyScanCallback) {
				this->energyScanCallback(this->lastChannelToEnergyScan);
				this->energyScanCallback = nullptr;  /* Disable callback */
			}
			if (this->activeScanCallback) {
				this->activeScanCallback(this->lastChannelToZigbeeNetworkScan);
				this->activeScanCallback = nullptr;  /* Disable callback */
			}
		}
	}
	break;
	case EZSP_TRUST_CENTER_JOIN_HANDLER: {
		clogI << "EZSP_TRUST_CENTER_JOIN_HANDLER" << " : " << i_msg_receive << "\n";
		EmberNodeId sender = static_cast<EmberNodeId>(dble_u8_to_u16(i_msg_receive.at(1U), i_msg_receive.at(0U)));
		uint8_t status = i_msg_receive.at(10U);
		EmberEUI64 eui64;
		for(uint8_t loop=0; loop < 8; loop++) {
			eui64[loop] = i_msg_receive.at(2U+loop);
		}

		if( nullptr != obsTrustCenterJoinHandlerCallback ) {
			obsTrustCenterJoinHandlerCallback(status, sender, eui64);
		}

	}
	break;
	case EZSP_GP_PROXY_TABLE_GET_ENTRY: {
		clogI << "EZSP_GP_PROXY_TABLE_GET_ENTRY" << " : " << i_msg_receive << "\n";
		
		NSSPI::ByteBuffer l_msg_raw;
		for(uint8_t loop = 0; loop < i_msg_receive.size()-1; loop++) {
			l_msg_raw.push_back(i_msg_receive.at(1U+loop));
		}
		CEmberGpProxyTableEntryStruct CEmberGpProxyTableEntry(l_msg_raw);
		CEmberGpAddressStruct CEmberGpAddress = CEmberGpProxyTableEntry.getGpdAddress();

		clogD << CEmberGpProxyTableEntry.getGpdAddress() << std::endl;

		if( nullptr != obsGpProxyTableEntryJoinHandlerCallback ) {
			obsGpProxyTableEntryJoinHandlerCallback(CEmberGpProxyTableEntry.getGpProxyTableEntryStatus(), CEmberGpAddress.getSourceId(), CEmberGpAddress.getApplicationId(), CEmberGpAddress.getEndpoint());
		}

	}
	break;
	case EZSP_INCOMING_MESSAGE_HANDLER:
	{
		//using namespace NSEZSP;
		// the most important function where all zigbee incomming message arrive
		EmberIncomingMessageType type = static_cast<EmberIncomingMessageType>(i_msg_receive.at(0));
		NSSPI::ByteBuffer l_aps_raw;
		uint8_t CAPSFrameSize = CAPSFrame::getSize();
		for(uint8_t loop = 0; loop < CAPSFrameSize; loop++) {
			l_aps_raw.push_back(i_msg_receive.at(1U+loop));
		}
		uint8_t last_hop_lqi = i_msg_receive.at(1U+CAPSFrameSize);
		uint8_t last_hop_rssi = i_msg_receive.at(2U+CAPSFrameSize);
		EmberNodeId sender = static_cast<EmberNodeId>(dble_u8_to_u16(i_msg_receive.at(4U+CAPSFrameSize), i_msg_receive.at(3U+CAPSFrameSize)));
		uint8_t binding_idx = i_msg_receive.at(5U+CAPSFrameSize);
		uint8_t address_idx = i_msg_receive.at(6U+CAPSFrameSize);
		uint8_t msg_length = i_msg_receive.at(7U+CAPSFrameSize);
		NSSPI::ByteBuffer l_msg_raw;
		for(uint8_t loop = 0; loop < msg_length; loop++) {
			l_msg_raw.push_back(i_msg_receive.at(8U+CAPSFrameSize+loop));
		}

		clogI << "EZSP_INCOMING_MESSAGE_HANDLER type : " << CEzspEnum::EmberIncomingMessageTypeToString(type) <<
			", last hop rssi : " << unsigned(last_hop_rssi) <<
			", from : "<< std::hex << std::setw(4) << std::setfill('0') << unsigned(sender) << std::endl;

		// Don't process some type of messages
		if( !((EMBER_INCOMING_BROADCAST_LOOPBACK==type) || (EMBER_INCOMING_MULTICAST_LOOPBACK==type) || (EMBER_INCOMING_MANY_TO_ONE_ROUTE_REQUEST==type)) ) {
			// Build a zigbee message to simplify process
			CZigBeeMsg zbMsg;
			zbMsg.Set(l_aps_raw,l_msg_raw);
			//clogI << std::hex << std::setw(2) << std::setfill('0') << zbMsg.GetAps().GetEmberAPS();
			if( 0 == zbMsg.GetAps().src_ep ) {
				EZdpLowByte zdp_low = static_cast<EZdpLowByte>(u16_get_lo_u8(zbMsg.GetAps().cluster_id));

				// ZDO (Zigbee Device Object)
				if( ZDP_HIGHT_BYTE_RESPONSE == u16_get_hi_u8(zbMsg.GetAps().cluster_id)) {
					switch(zdp_low) {
						case ZDP_ACTIVE_EP: {
							// byte 0 is for sequence number, ignore it!
							uint8_t status = zbMsg.GetPayload().at(1U);
							EmberNodeId address = static_cast<EmberNodeId>(dble_u8_to_u16(zbMsg.GetPayload().at(3U), zbMsg.GetPayload().at(2U)));
							uint8_t ep_count = zbMsg.GetPayload().at(4U);

							std::vector<uint8_t> ep_list;
							for(uint8_t loop = 0; loop < ep_count; loop++) {
								ep_list.push_back(zbMsg.GetPayload().at(5U+loop));
							}

							if( nullptr != obsZdpActiveEpRecvCallback ) {
								obsZdpActiveEpRecvCallback(status, address, ep_count, ep_list);
							}
						}
						break;
						case ZDP_SIMPLE_DESC: {
							// byte 0 is for sequence number, ignore it!
							uint8_t status = zbMsg.GetPayload().at(1U);

							if( 0 == status ) {
								EmberNodeId address = static_cast<EmberNodeId>(dble_u8_to_u16(zbMsg.GetPayload().at(3U), zbMsg.GetPayload().at(2U)));
								uint8_t lentgh = zbMsg.GetPayload().at(4U);
								uint8_t endpoint = zbMsg.GetPayload().at(5U);
								uint16_t profile_id = dble_u8_to_u16(zbMsg.GetPayload().at(7U), zbMsg.GetPayload().at(6U));
								uint16_t device_id = dble_u8_to_u16(zbMsg.GetPayload().at(9U), zbMsg.GetPayload().at(8U));
								uint8_t version = zbMsg.GetPayload().at(10U);
								uint8_t in_count = zbMsg.GetPayload().at(11U);
								uint8_t out_count = zbMsg.GetPayload().at(12U+(2U*in_count));

								std::vector<uint16_t> in_list;
								for(uint8_t loop = 0; loop < in_count; loop++) {
									in_list.push_back(dble_u8_to_u16(zbMsg.GetPayload().at(13U+(2U*loop)), zbMsg.GetPayload().at(12U+(2U*loop))));
								}

								std::vector<uint16_t> out_list;
								for(uint8_t loop=0; loop<out_count; loop++) {
									out_list.push_back(dble_u8_to_u16(zbMsg.GetPayload().at(14U+(2U*in_count)+(2U*loop)), zbMsg.GetPayload().at(13U+(2U*in_count)+(2U*loop))));
								}

								// You need to do your own binding in your application with ZDO command, none will be done here.

								if( nullptr != obsZdpActiveEpRecvCallback ) {
									obsZdpSimpleDescRecvCallback(status, address, endpoint, profile_id, device_id, version, in_count, out_count, in_list, out_list);
								}
							}
							else {
								clogI << CZdpEnum::ToString(zdp_low) << " Response with status : " <<
									std::hex << std::setw(2) << std::setfill('0') << unsigned(status) << std::endl;
							}
						}
						case ZDP_NWK_ADDR: {
							uint8_t status = zbMsg.GetPayload().at(1U);

							if( 0 == status ) {
								EmberNodeId address = static_cast<EmberNodeId>(dble_u8_to_u16(zbMsg.GetPayload().at(3U), zbMsg.GetPayload().at(2U)));
								std::stringstream buf;
								buf << CZdpEnum::ToString(zdp_low) << " Response : " <<
									"[ status : " << std::hex << std::setw(2) << std::setfill('0') << unsigned(status) << "]" <<
									"[ address : " << std::hex << std::setw(4) << std::setfill('0') << unsigned(address) << "]";
								//clogI << buf.str() << std::endl;
							}
							else {
								clogI << CZdpEnum::ToString(zdp_low) << " Response with status : " <<
									std::hex << std::setw(2) << std::setfill('0') << unsigned(status) << std::endl;
							}
							break;
						}
						break;
						case ZDP_BIND: {
							uint8_t status = zbMsg.GetPayload().at(1U);
							if( 0 == status ){
								std::stringstream buf;
								buf << CZdpEnum::ToString(zdp_low) << " Response : " <<
									"[ status : " << std::hex << std::setw(2) << std::setfill('0') << unsigned(status) << "]";
								clogI << buf.str() << std::endl;
							}
							else{
								clogI << CZdpEnum::ToString(zdp_low) << " Response with status : " <<
									std::hex << std::setw(2) << std::setfill('0') << unsigned(status) << std::endl;
							}
							break;
						}
						break;
						case ZDP_MGMT_BIND: {
							uint8_t status = zbMsg.GetPayload().at(1U);
							if( 0 == status ){
								std::stringstream buf;
								buf << CZdpEnum::ToString(zdp_low) << " Response : " <<
									"[ status : " << std::hex << std::setw(2) << std::setfill('0') << unsigned(status) << "]";
								clogI << buf.str() << std::endl;

								uint8_t bindingTableEntries = zbMsg.GetPayload().at(2U);
								uint8_t startIndex = zbMsg.GetPayload().at(3U);
								uint8_t bindingTableListCount = zbMsg.GetPayload().at(4U);
								int jump = 5U;
								NSEZSP::EmberEUI64 SrcAddr;
								std::vector<NSEZSP::MgmtBindRsp> bindingTable;

								if(bindingTableListCount != 0){
									for(uint8_t loop = 0; loop<bindingTableListCount; loop++){
										NSEZSP::MgmtBindRsp bindingEntry;
										for(uint8_t i=0; i < 8; i++) {
											SrcAddr[i] = zbMsg.GetPayload().at(jump+i);
										}
										bindingEntry.setSrcAddr(SrcAddr);
										bindingEntry.setEndpoint(zbMsg.GetPayload().at(jump+8U));
										bindingEntry.setCluster(dble_u8_to_u16(zbMsg.GetPayload().at(jump+10U), zbMsg.GetPayload().at(jump+9U)));
										bindingEntry.setDstAddrMode(zbMsg.GetPayload().at(jump+11U));

										if(bindingEntry.getDstAddrMode() == 0x03){
											NSEZSP::EmberEUI64 DstAddr;
											for(uint8_t i=0; i < 8; i++) {
												DstAddr[i] = zbMsg.GetPayload().at(jump+12U+i);
											}
											bindingEntry.setDstAddrEUI64(DstAddr);
											bindingEntry.setDstEndpoint(zbMsg.GetPayload().at(jump+20U));
											jump += 21U;
										}else{
											bindingEntry.setDstAddrNodeId(static_cast<EmberNodeId>(dble_u8_to_u16(zbMsg.GetPayload().at(jump+13U), zbMsg.GetPayload().at(jump+12U))));
											jump += 14U;
										}
										bindingTable.push_back(bindingEntry);	
									}
									
								}

								if( nullptr != obsBindingTableRecvCallback ) {
									obsBindingTableRecvCallback(sender, bindingTableEntries, startIndex, bindingTableListCount, bindingTable);
								}
							}
							else{
								clogI << CZdpEnum::ToString(zdp_low) << " Response with status : " <<
									std::hex << std::setw(2) << std::setfill('0') << unsigned(status) << std::endl;
							}
							break;
						}
						case ZDP_UNBIND: {
							uint8_t status = zbMsg.GetPayload().at(1U);
							if( 0 == status ){
								std::stringstream buf;
								buf << CZdpEnum::ToString(zdp_low) << " Response : " <<
									"[ status : " << std::hex << std::setw(2) << std::setfill('0') << unsigned(status) << "]";
								clogI << buf.str() << std::endl;
							}
							else{
								clogI << CZdpEnum::ToString(zdp_low) << " Response with status : " <<
									std::hex << std::setw(2) << std::setfill('0') << unsigned(status) << std::endl;
							}
							break;
						}
						break;
						default: {
							// DEBUG
							clogI << "ZDO Response : " << CZdpEnum::ToString(zdp_low) << std::endl;
							
						}
						break;
					}
				}
				else {
					if( ZDP_NODE_DESC == zdp_low) {
						clogI << "ZDO Request : " << CZdpEnum::ToString(zdp_low) << std::endl;
						
						//clogI << zbMsg.GetPayload() << std::endl;
						//EmberNodeId address = dble_u8_to_u16(zbMsg.GetPayload().at(1U), zbMsg.GetPayload().at(0U));

						//clogI << unsigned(address) << std::endl;
					}	
					else if( ZDP_DEVICE_ANNOUNCE == zdp_low ) {
						clogI << "ZDO Request : " << CZdpEnum::ToString(zdp_low) << std::endl;

						/* Dirty! Store announcing device address as global variable to able to use this value in binding request for example
						** Next step is to create Device, Enpoint, (Cluster ?) class to store all the dicovered info about product
						*/
						EmberEUI64 deviceEui64;
						for(uint8_t loop=0; loop < 8; loop++) {
							deviceEui64[loop] = zbMsg.GetPayload().at(3U+loop);
						}

						// We receive a device announce because a child join or rejoin network, start a discover endpoint process
						// byte 0 is for sequence number, ignore it!
						EmberNodeId address = dble_u8_to_u16(zbMsg.GetPayload().at(2U), zbMsg.GetPayload().at(1U));

						if( nullptr != obsZdpDeviceAnnounceRecvCallback ) {
							obsZdpDeviceAnnounceRecvCallback(address, deviceEui64);
						}
					}
					else if (ZDP_NWK_ADDR == zdp_low) {
						std::stringstream buf;
						buf << CZdpEnum::ToString(zdp_low) << " Response : " <<
							"[ address : " << std::hex << std::setw(4) << std::setfill('0') << unsigned(sender) << "]";
						clogI << buf.str() << std::endl;
					}
					else {
						// DEBUG
						clogI << "ZDO Request : " << CZdpEnum::ToString(zdp_low) << std::endl;
					}
				}
			}
			else {
				// Applicative zigbee message
				NSSPI::ByteBuffer data;

				uint8_t type = zbMsg.GetZCLHeader().GetFrmCtrl().convertEZCLFrameCtrlFrameTypeToInt8();
				for(uint8_t loop = 0; loop < zbMsg.GetPayload().size(); loop++) {
					data.push_back(zbMsg.GetPayload().at(loop));
					//buf << std::hex << std::setw(2) << std::setfill('0') << unsigned(zbMsg.GetPayload().at(loop)) << ", ";
				}
				NSEZSP::CZclFrame zcl_frame(zbMsg.GetAps().src_ep, zbMsg.GetAps().cluster_id, type, zbMsg.GetZCLHeader().GetCmdId(), data);

				if( nullptr != obsZclFrameRecvCallback ) {
					obsZclFrameRecvCallback(sender, zcl_frame, last_hop_lqi);
				}
			}
		}
	}
	break;
	default: {
		//DEBUG VIEW
		//clogI << "Unhandled EZSP message " << CEzspEnum::EEzspCmdToString(i_cmd) << ": " << i_msg_receive << "\n";
		
	}
	break;
	}
}

void CLibEzspMain::handleBootloaderPrompt() {
	clogI << "CLibEzspMain::handleBootloaderPrompt\n";
}

void CLibEzspMain::handleRxGpFrame( CGpFrame &i_gpf ) {
	// Start DEBUG
	clogI << "CLibEzspMain::handleRxGpFrame gp frame : " << i_gpf << std::endl;

	if( nullptr != obsGPFrameRecvCallback ) {
		obsGPFrameRecvCallback(i_gpf);
	}
}

void CLibEzspMain::handleRxGpdId( uint32_t &i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status ) {
	if( nullptr != obsGPSourceIdCallback ) {
		obsGPSourceIdCallback(i_gpd_id, i_gpd_known, i_gpd_key_status);
	}
}
