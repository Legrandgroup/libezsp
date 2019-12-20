#include "ezsp/ezsp.h"
#include "ezsp/lib-ezsp-main.h"

CEzsp::CEzsp(IUartDriver *uartDriver, TimerBuilder &timerbuilder)
{
	main = new CLibEzspMain(uartDriver, timerbuilder);
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
	main->clearAllGPDevices();
}

bool CEzsp::removeGPDevices(std::vector<uint32_t>& sourceIdList)
{
	main->removeGPDevices(sourceIdList);
}

bool CEzsp::addGPDevices(const std::vector<CGpDevice> &gpDevicesList)
{
	main->addGPDevices(gpDevicesList);
}

void CEzsp::setAnswerToGpfChannelRqstPolicy(bool allowed)
{
	main->setAnswerToGpfChannelRqstPolicy(allowed);
}
