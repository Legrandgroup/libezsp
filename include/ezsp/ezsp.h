#ifndef __EZSP_H__
#define __EZSP_H__

#include <functional>
#include <memory>
#include <vector>

#include <ezsp/gpd.h>
#include <ezsp/zbmessage/green-power-device.h>
#include <ezsp/zbmessage/green-power-frame.h>
#include <spi/TimerBuilder.h>
#include <spi/IUartDriver.h>

namespace NSEZSP {
/**
 * @brief Possible states of the state machine used by class CLibEzspMain
 */
enum class CLibEzspState {
    NO_INIT,                            /*<! Initial state, before starting. */
    READY,                              /*<! Library is ready to work and process new command */
    INIT_FAILED,                        /*<! Initialisation failed, Library is out of work */
    INIT_IN_PROGRESS,         /*<! We are starting up the Zigbee stack in the adapter */
    LEAVE_NWK_IN_PROGRESS, /*<! We are currently creating a new Zigbee network */
    FORM_NWK_IN_PROGRESS, /*<! We are currently leaving the Zigbee network we previously joined */
    SINK_BUSY,                          /*<! Enclosed sink is busy executing commands */
    SWITCH_TO_BOOTLOADER_IN_PROGRESS,   /*<! Switch to bootloader is pending */
};

class CLibEzspMain;

typedef std::function<void (CLibEzspState i_state)> FLibStateCallback;
typedef std::function<void (uint32_t &i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status)> FGpdSourceIdCallback;
typedef std::function<void (CGpFrame &i_gpf)> FGpFrameRecvCallback;

class CEzsp{
public:
	CEzsp(NSSPI::IUartDriver *uartDriver, NSSPI::TimerBuilder &timerbuilder, bool requestZbNetworkReset=false);

    /**
     * @brief Register callback on current library state
     *
     * @param newObsStateCallback A callback function of type void func(CLibEzspState& i_state), that will be invoked each time our internal state will change (or nullptr to disable callbacks)
     */
    void registerLibraryStateCallback(FLibStateCallback newObsStateCallback);

    /**
     * @brief Register callback to receive all incoming greenpower sourceId
     *
     * @param newObsGPFrameRecvCallback A callback function of type void func(CGpFrame &i_gpf), that will be invoked each time a new valid green power frame is received from a known source ID (or nullptr to disable callbacks)
     */
    void registerGPFrameRecvCallback(FGpFrameRecvCallback newObsGPFrameRecvCallback);

    /**
     * @brief Register callback to receive all incoming greenpower sourceId
     *
     * @param newObsGPSourceIdCallback A callback function of type void func(uint32_t &i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status), that will be invoked each time a new source ID transmits over the air (or nullptr to disable callbacks)
     */
    void registerGPSourceIdCallback(FGpdSourceIdCallback newObsGPSourceIdCallback);

    /**
     * @brief Remove GP all devices from sink
     *
     * After invoking this method, we won't process any message from any GP devices (but registerGPSourceIdCallback will still be triggered)
     *
     * @return true if the action is going to be run in the background, false if the sink is busy
     */
    bool clearAllGPDevices();

    /**
     * @brief Remove a specific set of GP devices from sink (identified by their source ID value)
     *
     * After invoking this method, we won't process any message from the GP devices provided as argument (but registerGPSourceIdCallback will still be triggered)
     *
     * @param sourceIdList A list of source IDs to remove
     *
     * @return true if the action is going to be run in the background, false if the sink is busy
     */
    bool removeGPDevices(std::vector<uint32_t>& sourceIdList);

    /**
     * @brief Add a specific set of GP devices from sink (identified by their source ID value+key)
     *
     * After invoking this method, we will start taking into acocunt all messages from the GP devices provided as argument
     *
     * @param gpDevicesList A list of CGpDevice objects containing source ID + key pairs to add
     *
     * @return true if the action is going to be run in the background, false if the sink is busy
     */
    bool addGPDevices(const std::vector<CGpDevice> &gpDevicesList);

    /**
     * @brief Controls the answer to request channel messages sent by GP devices
     *
     * @param allowed Set to true if answers to request channel is allowed
     */
    void setAnswerToGpfChannelRqstPolicy(bool allowed);

    /**
     * @brief Switch the embedded firmware to booloader prompt mode
     */
    void jumpToBootloader();

private:
	CLibEzspMain *main;
};
} //namespace NSEZSP 

#endif
