#include "ezsp/ezsp.h"
#include "ezsp/lib-ezsp-main.h"

using NSEZSP::CEzsp;

CEzsp::CEzsp(NSSPI::IUartDriver *uartDriver, NSSPI::TimerBuilder &timerbuilder)
{
#ifndef DYNAMIC_ALLOCATION
	static CLibEzspMain g_MainEzsp(uartDriver, timerbuilder);
	main = &g_MainEzsp;
#else
	main = new CLibEzspMain(uartDriver, timerbuilder);
#endif
}

void CEzsp::registerLibraryStateCallback(FGStateCallback newObsStateCallback)
{
	main->registerLibraryStateCallback(newObsStateCallback);
}

void CEzsp::registerGPSourceIdCallback(FGpdSourceIdCallback newObsGPSourceIdCallback)
{
	main->registerGPSourceIdCallback(newObsGPSourceIdCallback);
}

bool CEzsp::clearAllGPDevices()
{
	return main->clearAllGPDevices();
}

bool CEzsp::removeGPDevices(std::vector<uint32_t>& sourceIdList)
{
	return main->removeGPDevices(sourceIdList);
}

bool CEzsp::addGPDevices(const std::vector<CGpDevice> &gpDevicesList)
{
	return main->addGPDevices(gpDevicesList);
}

void CEzsp::setAnswerToGpfChannelRqstPolicy(bool allowed)
{
	main->setAnswerToGpfChannelRqstPolicy(allowed);
}

void CEzsp::jumpToBootloader()
{
	main->jumpToBootloader();
}
