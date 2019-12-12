/**
 * @file CAppDemo.h
 */

#pragma once

#include <vector>

#include "domain/ezsp-dongle.h"
#include "domain/zigbee-tools/zigbee-networking.h"
#include "domain/zigbee-tools/zigbee-messaging.h"
#include "domain/zigbee-tools/green-power-sink.h"
#include "domain/zbmessage/green-power-device.h"
#include "spi/IUartDriver.h"
#include "spi/ITimerFactory.h"
#include "spi/ILogger.h"
#include "dummy_db.h"

typedef enum
{
    APP_NOT_INIT, // starting state
    APP_READY,  // default state if no action in progress
    APP_ERROR,  // something wrong
    APP_INIT_IN_PROGRESS, // dongle init in progress
    APP_LEAVE_IN_PROGRESS, // leave a in progress
    APP_FORM_NWK_IN_PROGRESS // form a network in progress
}EAppState;

class CAppDemo : public CEzspDongleObserver, CGpObserver
{
public:
    /**
     * Constructor
     * FIXME: too many args, this constructor becomes unreadable
    **/
    CAppDemo(IUartDriver& uartDriver,
             ITimerFactory &i_timer_factory,
             bool reset=false,
             bool openGpCommissionning=false,
             uint8_t authorizeChannelRequestAnswerTimeout=0,
             bool openZigbeeCommissionning=false,
             unsigned int networkChannel=11,
             bool gpRemoveAllDevices=false,
             const std::vector<uint32_t>& gpDevicesToRemove={},
             const std::vector<CGpDevice>& gpDevicesList={});

    /**
     * Callback
     */
    void handleDongleState( EDongleState i_state );
    void handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive );
    void handleRxGpFrame( CGpFrame &i_gpf );
    void handleRxGpdId( uint32_t &i_gpd_id );

private:
    void setAppState( EAppState i_state );
    void dongleInit();
    void stackInit();
    void chRqstTimeout(void);

    static bool extractClusterReport( const std::vector<uint8_t >& payload, uint8_t& usedBytes );
    static bool extractMultiClusterReport( std::vector<uint8_t > payload );



private:
    std::unique_ptr<ITimer> timer;
    CEzspDongle dongle;
    CZigbeeMessaging zb_messaging;
    CZigbeeNetworking zb_nwk;
    CGpSink gp_sink;
    EAppState app_state;
    Cdb db;
    uint8_t ezsp_version;
    bool reset_wanted;	/*!< Do we reset the network and re-create a new one? */
    bool openGpCommissionningAtStartup;	/* Do we open GP commissionning at dongle initialization? */
    uint8_t authorizeChRqstAnswerTimeout; /* local flag to authorize answer to channel request, pass as a remaining time to accept in s */
    bool openZigbeeCommissionningAtStartup;	/* Do we open the Zigbee network at dongle initialization? */
    unsigned int channel;	/*!< The Zigbee channel on which to create the network (if reset_wanted is set) */
    bool removeAllGpds;	/*!< A flag to remove all GP devices from monitoring */
    std::vector<uint32_t> gpdToRemove;	/*!< A list of source IDs for GP devices to remove from previous monitoring */
    std::vector<CGpDevice> gpdList;	/*!< The list of GP devices we are monitoring */
};
