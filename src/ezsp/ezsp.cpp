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

void CEzsp::registerGPSourceIdCallback(FGpSourceIdCallback newObsGPSourceIdCallback) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	main->registerGPSourceIdCallback(newObsGPSourceIdCallback);
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

NSSPI::GenericAsyncDataInputObservable* CEzsp::getAdapterSerialReadObservable() {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	return main->getAdapterSerialReadObservable();
}
