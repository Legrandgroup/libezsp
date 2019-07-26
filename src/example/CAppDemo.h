/**
 * @file CAppDemo.h
 */

#pragma once

#include <vector>

#include "../domain/ezsp-dongle.h"
#include "../domain/zigbee-tools/zigbee-networking.h"
#include "../domain/zigbee-tools/zigbee-messaging.h"
#include "../domain/zigbee-tools/green-power-sink.h"
#include "../spi/IUartDriver.h"
#include "../spi/ITimerFactory.h"
#include "../spi/ILogger.h"
#include "dummy_db.h"

#define TEST_MQTT

#ifdef TEST_MQTT
    #include "mqtt.h"
#endif

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
    CAppDemo(IUartDriver& uartDriver, ITimerFactory &i_timer_factory, bool reset=false, unsigned int networkChannel=11, const std::vector<uint32_t>& sourceIdList={});
    ~CAppDemo();

    /**
     * Callback
     */
    void handleDongleState( EDongleState i_state );
    void handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive );
    void handleRxGpFrame( CGpFrame &i_gpf );

private:
    void setAppState( EAppState i_state );
    void dongleInit();
    void stackInit();

    static bool extractClusterReport( const std::vector<uint8_t >& payload, size_t& usedBytes, uint32_t sourceId, uint8_t linkValue, myMqtt& mqttPub );
    static bool extractMultiClusterReport( std::vector<uint8_t > payload, uint32_t sourceId, uint8_t linkValue, myMqtt& mqttPub );


private:
    CEzspDongle dongle;
    CZigbeeMessaging zb_messaging;
    CZigbeeNetworking zb_nwk;
    CGpSink gp_sink;
    EAppState app_state;
    Cdb db;
    uint8_t ezsp_version;
    bool reset_wanted;	/*!< Do we reset the network and re-create a new one? */
    unsigned int channel;	/*!< The Zigbee channel on which to create the network (if reset_wanted is set) */
#ifdef TEST_MQTT
    myMqtt mqtt_pub;
#endif
};

