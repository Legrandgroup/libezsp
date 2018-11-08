#ifndef EZSP_DONGLE_H
#define EZSP_DONGLE_H

#include <string>
#include <iostream>

#include "ezsp-enum.h"
#include "IUartDriver.h"
#include "ash.h"

class CEzspHandler
{
public:
    virtual void ezspTimerHdl();
    virtual void ezspStackStatusHdl( uint8_t i_status );
    virtual void ezspEnergyScanResultHdl( uint8_t i_channel, int8_t i_max_rssi );
    virtual void ezspNetworkFoundHdl( TNwkInfo i_nwk, uint8_t i_last_hop_lqi, int8_t i_last_hop_rssi );
    virtual void ezspScanCompleteHdl( void );
    virtual void ezspChildJoinHdl();
    virtual void ezspRemoteSetBindingHdl();
    virtual void ezspRemoteDeleteBindingHdl();
    virtual void ezspMessageSentUnicastHdl( EEmberStatus i_status, COutZbMessage *ip_out_msg );
    virtual void ezspMessageSentHdl( std::vector<uint8_t> i_rsp_param );
    virtual void ezspPollCompleteHdl();
    virtual void ezspPollHdl();
    virtual void ezspIncomingSenderEUI64Hdl();
    virtual void ezspIncomingMessageHdl( TInMsgInfo i_msg_info, CZigBeeMsg i_msg );
    virtual void ezspIncomingRouteRecordHdl();
    virtual void ezspIncomingManyToOneRouteRequestHdl();
    virtual void ezspIncomingRouteErrorHdl();
    virtual void ezspIdConflictHdl();
    virtual void ezspMacPassthroughMessageHdl();
    virtual void ezspMacFilterMatchMessageHdl( std::vector<uint8_t> l_msg );
    virtual void ezspRawTransmitCompleteHdl();
    virtual void ezspSwitchNetworkKeyHdl();
    virtual void ezspZigbeeKeyEstablishmentHdl();
    virtual void ezspTrustCenterJoinHdl();
    virtual void ezspGenerateCBKEKeysHdl();
    virtual void ezspCalculateSMACSHdl();
    virtual void ezspDSASignHdl();
    virtual void ezspDSAVerifyHdl();
    virtual void ezspMfglibRxHdl( std::vector<uint8_t> l_msg );
    virtual void ezspIncomingBootloadMessageHdl( uint64_t i_ieee, uint8_t i_last_lqi, int8_t l_last_rssi, vector<uint8_t> l_msg );
    virtual void ezspBootloadTransmitCompleteHdl();    
};


class CEzspDongle : public IAsyncDataInputObserver, public CAshCallback
{
public:
    CEzspDongle();

    /**
     * Open connetion to dongle of type ezsp
     */
    bool open(IUartDriver *ipUart);






    /**
     * callback de reception de l'uart
     */
    virtual void handleInputData(const unsigned char* dataIn, const size_t dataLen);

    /**
     * callabck d'information de l'ash
     */
    virtual void ashCbInfo( EAshInfo info ) { std::cout <<  "ashCbInfo : " << info << std::endl; };

private:
    IUartDriver *pUart;
    CAsh *ash;
    CEzspHandler *pHandler;

};

#endif // EZSP_DONGLE_H