#ifndef EZSP_DONGLE_H
#define EZSP_DONGLE_H

#include <string>
#include <iostream>

#include "ezsp-enum.h"
#include "IUartDriver.h"
#include "ash.h"
#include "zigbee-message.h"
#include "out-zigbee-message.h"

class CEzspHandler
{
public:
    virtual void ezspTimerHdl() = 0;
    virtual void ezspStackStatusHdl( uint8_t i_status ) = 0;
    virtual void ezspEnergyScanResultHdl( uint8_t i_channel, int8_t i_max_rssi ) = 0;
    virtual void ezspNetworkFoundHdl( TNwkInfo i_nwk, uint8_t i_last_hop_lqi, int8_t i_last_hop_rssi ) = 0;
    virtual void ezspScanCompleteHdl( void ) = 0;
    virtual void ezspChildJoinHdl() = 0;
    virtual void ezspRemoteSetBindingHdl() = 0;
    virtual void ezspRemoteDeleteBindingHdl() = 0;
    virtual void ezspMessageSentUnicastHdl( EEmberStatus i_status, COutZbMessage *ip_out_msg ) = 0;
    virtual void ezspMessageSentHdl( std::vector<uint8_t> i_rsp_param ) = 0;
    virtual void ezspPollCompleteHdl() = 0;
    virtual void ezspPollHdl() = 0;
    virtual void ezspIncomingSenderEUI64Hdl() = 0;
    virtual void ezspIncomingMessageHdl( TInMsgInfo i_msg_info, CZigBeeMsg i_msg ) = 0;
    virtual void ezspIncomingRouteRecordHdl() = 0;
    virtual void ezspIncomingManyToOneRouteRequestHdl() = 0;
    virtual void ezspIncomingRouteErrorHdl() = 0;
    virtual void ezspIdConflictHdl() = 0;
    virtual void ezspMacPassthroughMessageHdl() = 0;
    virtual void ezspMacFilterMatchMessageHdl( std::vector<uint8_t> l_msg ) = 0;
    virtual void ezspRawTransmitCompleteHdl() = 0;
    virtual void ezspSwitchNetworkKeyHdl() = 0;
    virtual void ezspZigbeeKeyEstablishmentHdl() = 0;
    virtual void ezspTrustCenterJoinHdl() = 0;
    virtual void ezspGenerateCBKEKeysHdl() = 0;
    virtual void ezspCalculateSMACSHdl() = 0;
    virtual void ezspDSASignHdl() = 0;
    virtual void ezspDSAVerifyHdl() = 0;
    virtual void ezspMfglibRxHdl( std::vector<uint8_t> l_msg ) = 0;
    virtual void ezspIncomingBootloadMessageHdl( uint64_t i_ieee, uint8_t i_last_lqi, int8_t l_last_rssi, std::vector<uint8_t> l_msg ) = 0;
    virtual void ezspBootloadTransmitCompleteHdl() = 0;
};


class CEzspDongle : public IAsyncDataInputObserver, public CAshCallback
{
public:
    CEzspDongle( CEzspHandler *ipCb );
    ~CEzspDongle();

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
    CEzspHandler *pCb;

};

#endif // EZSP_DONGLE_H