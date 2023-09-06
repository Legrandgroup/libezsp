/**
 * @file ezsp.cpp
 *
 * @brief Facade for NSEZSP::CLibEzspMain, hiding its internals from the published headers
 *
 * @note In order to verbosely trace all API calls, use compiler directive TRACE_API_CALLS when compiling this code
 */

//#define TRACE_API_CALLS
#define DYNAMIC_ALLOCATION

#include <sstream>
#include <iomanip>

#include "ezsp/ezsp.h"
#include "ezsp/lib-ezsp-main.h"
#include "ezsp/byte-manip.h"
#ifdef TRACE_API_CALLS
#include "spi/ILogger.h"
#endif

#include "ezsp/ezsp-protocol/struct/ember-zigbee-network.h"	// For CEmberZigbeeNetwork

DEFINE_ENUM(State, CLIBEZSP_STATE_LIST, NSEZSP::CLibEzspPublic);

using NSEZSP::ZigbeeNetworkScanResult;

ZigbeeNetworkScanResult::ZigbeeNetworkScanResult(NSEZSP::CEmberZigbeeNetwork& networkDetails, uint8_t lastHopLqi, int8_t lastHopRssi) :
	networkDetails(new NSEZSP::CEmberZigbeeNetwork(networkDetails)),
	lastHopLqi(lastHopLqi),
	lastHopRssi(lastHopRssi) {
}

ZigbeeNetworkScanResult::ZigbeeNetworkScanResult(const ZigbeeNetworkScanResult& other) :
	networkDetails(new NSEZSP::CEmberZigbeeNetwork(*(other.networkDetails))),
	lastHopLqi(other.lastHopLqi),
	lastHopRssi(other.lastHopRssi) {
}

ZigbeeNetworkScanResult::~ZigbeeNetworkScanResult() {
	delete this->networkDetails;
}

uint16_t ZigbeeNetworkScanResult::getPanId() const {
	return this->networkDetails->panId;
}

uint64_t ZigbeeNetworkScanResult::getExtendedPanId() const {
	return this->networkDetails->extendedPanId;
}

std::string ZigbeeNetworkScanResult::toString() const {
	std::stringstream buf;

	buf << "ZigbeeNetworkScanResult: { ";
	buf << "[channel: " << std::dec << std::setw(0) << static_cast<unsigned int>(this->networkDetails->channel) << "]";
	buf << "[lastHop: LQI="  << std::dec << std::setw(0) << static_cast<unsigned int>(this->lastHopLqi) << ", rssi=" << static_cast<int>(this->lastHopRssi) << "]";
	buf << "[panId: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(this->networkDetails->panId) << "]";
	buf << "[extendedPanId: 0x" << std::hex
	    << std::setw(8) << std::setfill('0') << u64_get_hi_u32(this->networkDetails->extendedPanId)
	    << std::setw(8) << std::setfill('0') << u64_get_lo_u32(this->networkDetails->extendedPanId) << "]";
	buf << "[allowingJoin: " << std::string(this->networkDetails->allowingJoin?"true":"false") << "]";
	buf << "[stackProfile: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(this->networkDetails->stackProfile) << "]";
	buf << "[nwkUpdateId: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(this->networkDetails->nwkUpdateId) << "]";
	buf << " }";

	return buf.str();
}

using NSEZSP::CEzsp;

CEzsp::CEzsp(NSSPI::IUartDriverHandle uartHandle, const NSSPI::TimerBuilder& timerbuilder, unsigned int requestZbNetworkResetToChannel) :
	main(nullptr) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << " constructor on instance " << static_cast<void *>(this) << "\n";
#endif
#ifndef DYNAMIC_ALLOCATION
	static CLibEzspMain g_MainEzsp(uartHandle, timerbuilder, requestZbNetworkResetToChannel);
	main = &g_MainEzsp;
#else
	main = new CLibEzspMain(uartHandle, timerbuilder, requestZbNetworkResetToChannel);
#endif
}

CEzsp::~CEzsp() {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << " destructor on instance " << static_cast<void *>(this) << "\n";
#endif
#ifndef DYNAMIC_ALLOCATION
#else
	free(main);
#endif
}

void CEzsp::start() {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "\n";
#endif
	main->start();
}

NSEZSP::EzspAdapterVersion CEzsp::getAdapterVersion() const {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	return main->getAdapterVersion();
}

void CEzsp::forceFirmwareUpgradeOnInitTimeout() {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	main->forceFirmwareUpgradeOnInitTimeout();
}

void CEzsp::registerLibraryStateCallback(FLibStateCallback newObsStateCallback) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	main->registerLibraryStateCallback(newObsStateCallback);
}

void CEzsp::registerGPFrameRecvCallback(FGpFrameRecvCallback newObsStateCallback) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	main->registerGPFrameRecvCallback(newObsStateCallback);
}

void CEzsp::registerGPFrameCommissioningRecvCallback(FGpFrameCommissioningRecvCallback newObsGPFrameCommissioningRecvCallback){
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	main->registerGPFrameCommissioningRecvCallback(newObsGPFrameCommissioningRecvCallback);
}

void CEzsp::registerGPSourceIdCallback(FGpSourceIdCallback newObsGPSourceIdCallback) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	main->registerGPSourceIdCallback(newObsGPSourceIdCallback);
}

void CEzsp::registerZclFrameRecvCallback(FZclFrameRecvCallback newObsZclFrameRecvCallback){
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	main->registerZclFrameRecvCallback(newObsZclFrameRecvCallback);
}

void CEzsp::registerBindingTableRecvCallback(FBindingTableRecvCallback newObsBindingTableRecvCallback){
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	main->registerBindingTableRecvCallback(newObsBindingTableRecvCallback);
}

void CEzsp::registerZdpDeviceAnnounceRecvCallback(FZdpDeviceAnnounceCallBack newObsZdpDeviceAnnounceRecvCallback){
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	main->registerZdpDeviceAnnounceRecvCallback(newObsZdpDeviceAnnounceRecvCallback);
}

void CEzsp::registerZdpActiveEpRecvCallback(FZdpActiveEpCallBack newObsZdpActiveEpRecvCallback){
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	main->registerZdpActiveEpRecvCallback(newObsZdpActiveEpRecvCallback);
}

void CEzsp::registerTrustCenterJoinHandlerCallback(FTrustCenterJoinHandlerCallBack newObsTrustCenterJoinHandlerCallback){
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	main->registerTrustCenterJoinHandlerCallback(newObsTrustCenterJoinHandlerCallback);
}

void CEzsp::registerGpProxyTableEntryJoinHandlerCallback(FGpProxyTableEntryHandlerCallBack newObsGpProxyTableEntryJoinHandlerCallback){
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	main->registerGpProxyTableEntryJoinHandlerCallback(newObsGpProxyTableEntryJoinHandlerCallback);
}

void CEzsp::registerDongleEUI64RecvCallback(FDongleEUI64CallBack newObsDongleEUI64RecvCallback){
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	main->registerDongleEUI64RecvCallback(newObsDongleEUI64RecvCallback);
}

void CEzsp::registerZdpSimpleDescRecvCallback(FZdpSimpleDescCallBack newObsZdpSimpleDescRecvCallback){
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	main->registerZdpSimpleDescRecvCallback(newObsZdpSimpleDescRecvCallback);
}

void CEzsp::registerNetworkParametersCallback(FNetworkParametersCallback newObsNetworkParametersCallback){
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	main->registerNetworkParametersCallback(newObsNetworkParametersCallback);
}

bool CEzsp::getEUI64() {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	return main->getEUI64();
}

bool CEzsp::getGPProxyTableEntry(const int index) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	return main->getGPProxyTableEntry(index);
}

bool CEzsp::clearAllGPDevices() {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	return main->clearAllGPDevices();
}

bool CEzsp::removeGPDevices(const std::vector<uint32_t>& sourceIdList) {
#ifdef TRACE_API_CALLS
	std::stringstream sourceIdListAsStr;
	for (auto it = sourceIdList.begin(); it != sourceIdList.end(); ++it) {
		if (it != sourceIdList.begin()) {
			sourceIdListAsStr << ",";
		}
		sourceIdListAsStr << *it;
	}
	clogD << "->API call " << __func__ << "([" << sourceIdListAsStr.str() << "])\n";
#endif
	return main->removeGPDevices(sourceIdList);
}

bool CEzsp::addGPDevices(const std::vector<CGpDevice> &gpDevicesList) {
#ifdef TRACE_API_CALLS
	std::stringstream gpDevicesListAsStr;
	for (auto it = gpDevicesList.begin(); it != gpDevicesList.end(); ++it) {
		if (it != gpDevicesList.begin()) {
			gpDevicesListAsStr << ",";
		}
		gpDevicesListAsStr << "0x" << std::hex << std::setw(8) << std::setfill('0') << it->getSourceId();
	}
	clogD << "->API call " << __func__ << "([" << gpDevicesListAsStr.str() << "])\n";
#endif
	return main->addGPDevices(gpDevicesList);
}

bool CEzsp::openCommissioningSession() {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	return main->openCommissioningSession();
}

bool CEzsp::closeCommissioningSession() {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	return main->closeCommissioningSession();
}

void CEzsp::setAnswerToGpfChannelRqstPolicy(bool allowed) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "(" << (allowed?"true":"false") << ")\n";
#endif
	main->setAnswerToGpfChannelRqstPolicy(allowed);
}

void CEzsp::setFirmwareUpgradeMode() {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	main->setFirmwareUpgradeMode();
}

bool CEzsp::startEnergyScan(FEnergyScanCallback energyScanCallback, uint8_t duration, uint32_t requestedChannelMask) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "({callback}," << std::dec << static_cast<unsigned int>(duration);
	if (requestedChannelMask) {
		clogD << ",0x" << std::hex << static_cast<unsigned int>(requestedChannelMask);
	}
	clogD << ")\n";
#endif
	return main->startEnergyScan(energyScanCallback, duration, requestedChannelMask);
}

bool CEzsp::startActiveScan(FActiveScanCallback activeScanCallback, uint8_t duration, uint32_t requestedChannelMask) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "({callback}," << std::dec << static_cast<unsigned int>(duration);
	if (requestedChannelMask) {
		clogD << ",0x" << std::hex << static_cast<unsigned int>(requestedChannelMask);
	}
	clogD << ")\n";
#endif
	return main->startActiveScan(activeScanCallback, duration, requestedChannelMask);
}


bool CEzsp::getNetworkKey(FNetworkKeyCallback networkKeyCallback) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	return main->getNetworkKey(networkKeyCallback);
}

bool CEzsp::setChannel(uint8_t channel) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "(" << std::dec << static_cast<unsigned int>(channel) << ")\n";
#endif
	return main->setChannel(channel);
}

bool CEzsp::joinNetwork(const uint64_t extendedPanId,
                        const uint16_t panId,
                        const uint8_t channel,
                        const NSEZSP::EmberJoinMethod joinMethod,
                        const uint32_t allowedChannels,
                        const uint8_t radioTxPower,
                        const NSEZSP::EmberNodeId nwkManagerId,
                        const uint8_t nwkUpdateId) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "(" << std::hex << std::setfill('0') << std::setw(16) << extendedPanId << ", " << std::setw(4) << panId << ", ...)\n";
#endif
	NSEZSP::CEmberNetworkParameters nwkParams;

	nwkParams.setExtendPanId(extendedPanId);
	nwkParams.setPanId(panId);
	nwkParams.setJoinMethod(joinMethod);
	nwkParams.setRadioChannel(channel);
	nwkParams.setChannels(allowedChannels);
	nwkParams.setRadioTxPower(radioTxPower);
	nwkParams.setNwkManagerId(nwkManagerId);
	nwkParams.setNwkUpdateId(nwkUpdateId);
	
	return main->joinNetwork(nwkParams);
}

bool CEzsp::createNetwork(uint8_t channel){
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "(" << std::dec << static_cast<unsigned int>(i_timeout) << ")\n";
#endif
	return main->createNetwork(channel);
}

bool CEzsp::openNetwork(uint8_t i_timeout) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "(" << std::dec << static_cast<unsigned int>(i_timeout) << ")\n";
#endif
	return main->openNetwork(i_timeout);
}

bool CEzsp::closeNetwork() {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "(" << std::dec << static_cast<unsigned int>(i_timeout) << ")\n";
#endif
	return main->closeNetwork();
}

bool CEzsp::SendZDOCommand(EmberNodeId i_node_id, uint16_t i_cmd_id, const NSSPI::ByteBuffer& payload) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	return main->SendZDOCommand(i_node_id, i_cmd_id, payload);
}

bool CEzsp::SendZCLCommand(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint8_t i_cmd_id,
						   const NSEZSP::EZCLFrameCtrlDirection i_direction, const NSSPI::ByteBuffer& i_payload,
						   const uint16_t i_node_id, const uint8_t i_transaction_number,
						   const uint16_t i_grp_id, const uint16_t i_manufacturer_code) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	return main->SendZCLCommand(i_endpoint, i_cluster_id, i_cmd_id, i_direction, i_payload, i_node_id, i_transaction_number, i_grp_id, i_manufacturer_code);
}

bool CEzsp::DiscoverAttributes(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint16_t i_start_attribute_identifier,
							  const uint8_t i_maximum_attribute_identifier, const EZCLFrameCtrlDirection i_direction, const uint16_t i_node_id,
							  const uint8_t i_transaction_number, const uint16_t i_grp_id, const uint16_t i_manufacturer_code){
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	return main->DiscoverAttributes(i_endpoint, i_cluster_id, i_start_attribute_identifier, i_maximum_attribute_identifier, i_direction, i_node_id, i_transaction_number, i_grp_id, i_manufacturer_code);
}

bool CEzsp::ReadAttributes(const uint8_t i_endpoint, const uint16_t i_cluster_id, const std::vector<uint16_t> &i_attribute_ids,
				   const EZCLFrameCtrlDirection i_direction, const uint16_t i_node_id,
				   const uint8_t i_transaction_number, const uint16_t i_grp_id, const uint16_t i_manufacturer_code){
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	return main->ReadAttributes(i_endpoint, i_cluster_id, i_attribute_ids, i_direction, i_node_id, i_transaction_number, i_grp_id, i_manufacturer_code);
}

bool CEzsp::WriteAttribute(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint16_t i_attribute_id,
									  const EZCLFrameCtrlDirection i_direction, const uint8_t i_datatype, const NSSPI::ByteBuffer& i_data,
									  const uint16_t i_node_id, const uint8_t i_transaction_number,
									  const uint16_t i_grp_id, const uint16_t i_manufacturer_code) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	return main->WriteAttribute(i_endpoint, i_cluster_id, i_attribute_id, i_direction, i_datatype, i_data, i_node_id, i_transaction_number, i_grp_id, i_manufacturer_code);
}

bool CEzsp::ConfigureReporting(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint16_t i_attribute_id,
							   const EZCLFrameCtrlDirection i_direction, const uint8_t i_datatype, const uint16_t i_min,
							   const uint16_t i_max, const uint16_t i_reportable, const uint16_t i_node_id,
							   const uint8_t i_transaction_number, const uint16_t i_grp_id, const uint16_t i_manufacturer_code) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	return main->ConfigureReporting(i_endpoint, i_cluster_id, i_attribute_id, i_direction, i_datatype, i_min, i_max, i_reportable, i_node_id, i_transaction_number, i_grp_id, i_manufacturer_code);
}

NSSPI::GenericAsyncDataInputObservable* CEzsp::getAdapterSerialReadObservable() {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	return main->getAdapterSerialReadObservable();
}
