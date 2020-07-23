/**
 * @file ezsp.cpp
 * 
 * @brief Facade for NSEZSP::CLibEzspMain, hiding its internals from the published headers
 * 
 * @note In order to verbosely trace all API calls, use compiler directive TRACE_API_CALLS when compiling this code
 */

//#define TRACE_API_CALLS
#define DYNAMIC_ALLOCATION

#include "ezsp/ezsp.h"
#include "ezsp/lib-ezsp-main.h"
#ifdef TRACE_API_CALLS
#include <sstream>
#include <iomanip>
#include "spi/ILogger.h"
#endif

DEFINE_ENUM(State, CLIBEZSP_STATE_LIST, NSEZSP::CLibEzspPublic);

using NSEZSP::CEzsp;

CEzsp::CEzsp(NSSPI::IUartDriverHandle uartHandle, const NSSPI::TimerBuilder& timerbuilder, unsigned int requestZbNetworkResetToChannel) {
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

bool CEzsp::openCommissioningSession(){
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

bool CEzsp::startEnergyScan(FEnergyScanCallback energyScanCallback, uint8_t duration) {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "({callback}," << std::dec << static_cast<unsigned int>(duration) << ")\n";
#endif
	return main->startEnergyScan(energyScanCallback, duration);
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

NSSPI::GenericAsyncDataInputObservable* CEzsp::getAdapterSerialReadObservable() {
#ifdef TRACE_API_CALLS
	clogD << "->API call " << __func__ << "()\n";
#endif
	return main->getAdapterSerialReadObservable();
}
