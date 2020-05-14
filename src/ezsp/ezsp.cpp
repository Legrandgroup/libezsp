/**
 * @file ezsp.cpp
 * 
 * @brief Facade for NSEZSP::CLibEzspMain, hiding its internals in the published headers
 */

#include "ezsp/ezsp.h"
#include "ezsp/lib-ezsp-main.h"

DEFINE_ENUM(State, CLIBEZSP_STATE_LIST, NSEZSP::CLibEzspPublic);

using NSEZSP::CEzsp;

CEzsp::CEzsp(NSSPI::IUartDriverHandle uartHandle, const NSSPI::TimerBuilder& timerbuilder, unsigned int requestZbNetworkResetToChannel)
{
#ifndef DYNAMIC_ALLOCATION
	static CLibEzspMain g_MainEzsp(uartHandle, timerbuilder, requestZbNetworkResetToChannel);
	main = &g_MainEzsp;
#else
	main = new CLibEzspMain(uartHandle, timerbuilder, requestZbNetworkResetToChannel);

	/* FIXME: Memory leak here, if DYNAMIC_ALLOCATION is defined, main is allocated but never de-allocated, change this to a std::unique_ptr */
#endif
}

void CEzsp::start()
{
	main->start();
}

NSEZSP::EzspAdapterVersion CEzsp::getAdapterVersion() const {
	return main->getAdapterVersion();
}

void CEzsp::forceFirmwareUpgradeOnInitTimeout()
{
	main->forceFirmwareUpgradeOnInitTimeout();
}

void CEzsp::registerLibraryStateCallback(FLibStateCallback newObsStateCallback)
{
	main->registerLibraryStateCallback(newObsStateCallback);
}

void CEzsp::registerGPFrameRecvCallback(FGpFrameRecvCallback newObsStateCallback)
{
	main->registerGPFrameRecvCallback(newObsStateCallback);
}

void CEzsp::registerGPSourceIdCallback(FGpSourceIdCallback newObsGPSourceIdCallback)
{
	main->registerGPSourceIdCallback(newObsGPSourceIdCallback);
}

bool CEzsp::clearAllGPDevices()
{
	return main->clearAllGPDevices();
}

bool CEzsp::removeGPDevices(const std::vector<uint32_t>& sourceIdList)
{
	return main->removeGPDevices(sourceIdList);
}

bool CEzsp::addGPDevices(const std::vector<CGpDevice> &gpDevicesList)
{
	return main->addGPDevices(gpDevicesList);
}

bool CEzsp::openCommissioningSession()
{
	return main->openCommissioningSession();
}

bool CEzsp::closeCommissioningSession()
{
	return main->closeCommissioningSession();
}

void CEzsp::setAnswerToGpfChannelRqstPolicy(bool allowed)
{
	main->setAnswerToGpfChannelRqstPolicy(allowed);
}

void CEzsp::setFirmwareUpgradeMode()
{
	main->setFirmwareUpgradeMode();
}

bool CEzsp::startEnergyScan(FEnergyScanCallback energyScanCallback, uint8_t duration)
{
	return main->startEnergyScan(energyScanCallback, duration);
}

bool CEzsp::setChannel(uint8_t channel) {
	return main->setChannel(channel);
}
