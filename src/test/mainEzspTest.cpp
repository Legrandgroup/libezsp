

#include <iostream>

#include "../domain/ezsp-dongle.h"
#include "../spi/raritan/RaritanEventLoop.h"
#include "../spi/raritan/RaritanUartDriver.h"
#include "../spi/IUartDriver.h"

using namespace std;

class CAppDemo : public CEzspHandler
{
public:
    CAppDemo(RaritanEventLoop* eventLoop, IUartDriver& uartDriver) : dongle(this), eventLoop(eventLoop) { 
        // uart
        dongle.open(&uartDriver);
    }

    ~CAppDemo() { }

    void loop(void) {
        if (eventLoop) {
            eventLoop->run();
        }
        else {
            while(1)
                ;
        }
    }

    /**
     * Callback
     */
    virtual void ezspTimerHdl(){;}
    virtual void ezspStackStatusHdl( uint8_t i_status ){;}
    virtual void ezspEnergyScanResultHdl( uint8_t i_channel, int8_t i_max_rssi ){;}
    virtual void ezspNetworkFoundHdl( TNwkInfo i_nwk, uint8_t i_last_hop_lqi, int8_t i_last_hop_rssi ){;}
    virtual void ezspScanCompleteHdl( void ){;}
    virtual void ezspChildJoinHdl(){;}
    virtual void ezspRemoteSetBindingHdl(){;}
    virtual void ezspRemoteDeleteBindingHdl(){;}
    virtual void ezspMessageSentUnicastHdl( EEmberStatus i_status, COutZbMessage *ip_out_msg ){;}
    virtual void ezspMessageSentHdl( std::vector<uint8_t> i_rsp_param ){;}
    virtual void ezspPollCompleteHdl(){;}
    virtual void ezspPollHdl(){;}
    virtual void ezspIncomingSenderEUI64Hdl(){;}
    virtual void ezspIncomingMessageHdl( TInMsgInfo i_msg_info, CZigBeeMsg i_msg ){;}
    virtual void ezspIncomingRouteRecordHdl(){;}
    virtual void ezspIncomingManyToOneRouteRequestHdl(){;}
    virtual void ezspIncomingRouteErrorHdl(){;}
    virtual void ezspIdConflictHdl(){;}
    virtual void ezspMacPassthroughMessageHdl(){;}
    virtual void ezspMacFilterMatchMessageHdl( std::vector<uint8_t> l_msg ){;}
    virtual void ezspRawTransmitCompleteHdl(){;}
    virtual void ezspSwitchNetworkKeyHdl(){;}
    virtual void ezspZigbeeKeyEstablishmentHdl(){;}
    virtual void ezspTrustCenterJoinHdl(){;}
    virtual void ezspGenerateCBKEKeysHdl(){;}
    virtual void ezspCalculateSMACSHdl(){;}
    virtual void ezspDSASignHdl(){;}
    virtual void ezspDSAVerifyHdl(){;}
    virtual void ezspMfglibRxHdl( std::vector<uint8_t> l_msg ){;}
    virtual void ezspIncomingBootloadMessageHdl( uint64_t i_ieee, uint8_t i_last_lqi, int8_t l_last_rssi, std::vector<uint8_t> l_msg ){;}
    virtual void ezspBootloadTransmitCompleteHdl(){;}

private:
    CEzspDongle dongle;
    RaritanEventLoop* eventLoop;
};


int main( void )
{
    CAppDemo *app;
    RaritanEventLoop eventLoop;
    UartDriverRaritan uartDriver(eventLoop);

    cout << "Starting ezsp test program !" << endl;

    uartDriver.open("/dev/ttyUSB0", 57600);

    app = new CAppDemo(&eventLoop, uartDriver);

    app->loop();

    delete app;

    return 0;
}