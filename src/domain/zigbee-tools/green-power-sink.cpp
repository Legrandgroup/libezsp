/**
 * @file green-power-sink.cpp
 */

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <map>

#include "green-power-sink.h"
#include "../ezsp-protocol/struct/ember-gp-address-struct.h"

#include "../../domain/zbmessage/zigbee-message.h"

#include "../../spi/GenericLogger.h"
#include "../../spi/ILogger.h"

// some defines to help understanding
#define GP_ENDPOINT 242

// cluster
#define GP_CLUSTER_ID   0x0021
// receive client command
#define GP_PROXY_COMMISIONING_MODE_CLIENT_CMD_ID    0x02




CGpSink::CGpSink( CEzspDongle &i_dongle, CZigbeeMessaging &i_zb_messaging ) :
    dongle(i_dongle),
    zb_messaging(i_zb_messaging),
    sink_table(),
    observers()
{
    dongle.registerObserver(this);
}

void CGpSink::init(void)
{
    // initialize green power sink
    clogD << "Call EZSP_GP_SINK_TABLE_INIT" << std::endl;
    dongle.sendCommand(EZSP_GP_SINK_TABLE_INIT);
}

void CGpSink::openCommissioningSession(void)
{
    // set local proxy in commissioning mode
    sendLocalGPProxyCommissioningMode();
}

uint8_t CGpSink::registerGpd( uint32_t i_source_id )
{
    CGpSinkTableEntry l_entry = CGpSinkTableEntry(i_source_id);

    return sink_table.addEntry(l_entry);
}

void CGpSink::handleDongleState( EDongleState i_state )
{
}

void CGpSink::handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive )
{
    switch( i_cmd )
    {
        case EZSP_GP_SINK_TABLE_INIT:
        {
            clogI << "EZSP_GP_SINK_TABLE_INIT RSP" << std::endl;
        }
        break;        
        case EZSP_GPEP_INCOMING_MESSAGE_HANDLER:
        {
            EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));

            // build gpf frame from ezsp rx message
            CGpFrame gpf = CGpFrame(i_msg_receive);


            // Start DEBUG
            clogD << "EZSP_GPEP_INCOMING_MESSAGE_HANDLER status : " << CEzspEnum::EEmberStatusToString(l_status) <<
                ", link : " << unsigned(i_msg_receive.at(1)) <<
                ", sequence number : " << unsigned(i_msg_receive.at(2)) <<
                ", gp address : " << gpf <</*
                ", last hop rssi : " << unsigned(last_hop_rssi) << 
                ", from : "<< std::hex << std::setw(4) << std::setfill('0') << unsigned(sender) << */
                std::endl;

/*
            std::stringstream bufDump;
            for (size_t i =0; i<i_msg_receive.size(); i++) {
                bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[i]) << " ";
            }
            clogI << "raw : " << bufDump.str() << std::endl;
*/
            // Stop DEBUG

            /**
             * trame gpf:
             * - no cryptée : on essaye de la validé en donner la TC link key (zig...009), dans le cas ou il s'agit d'une trame de commissioning
             * - crypté :
             *      - on cherche dans la table du sink (créé une class sink table entry comme décrit tabnle 25 section A3.3.2.2.2 du doc Green Power Basic spec v1.0)
             *      - si trouvé on essaye de la validé en passant la key associéé
             * - si la validation réussi on notifie le gpf au observateur
             */

            if( GPD_NO_SECURITY == gpf.getSecurity() )
            {
                // to test notify
                notifyObserversOfRxGpFrame( gpf );
            }
            else
            {
                // look up if product is register
                uint8_t l_sink_entry_idx = sink_table.getEntryIndexForSourceId( gpf.getSourceId() );
                if( GP_SINK_INVALID_ENTRY != l_sink_entry_idx )
                {
                    // to test notify
                    notifyObserversOfRxGpFrame( gpf );
                }
            }
        }
        break;

        default:
        {
            /* DEBUG VIEW
            std::stringstream bufDump;

            for (size_t i =0; i<i_msg_receive.size(); i++) {
                bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[i]) << " ";
            }
            clogI << "CGpSink::ezspHandler : " << bufDump.str() << std::endl;
            */
        }
        break;
    }
}

bool CGpSink::registerObserver(CGpObserver* observer)
{
    return this->observers.emplace(observer).second;
}

bool CGpSink::unregisterObserver(CGpObserver* observer)
{
    return static_cast<bool>(this->observers.erase(observer));
}

void CGpSink::notifyObserversOfRxGpFrame( CGpFrame i_gpf ) {
    for(auto observer : this->observers) {
        observer->handleRxGpFrame( i_gpf );
    }
}

void CGpSink::sendLocalGPProxyCommissioningMode(void)
{
    // forge GP Proxy Commissioning Mode command
    // assume we are coordinator of network and our nodeId is 0

    CZigBeeMsg l_gp_comm_msg;
    std::vector<uint8_t> l_gp_comm_payload;

    // options:
    // bit0 (Action) : 0b1 / request to enter commissioning mode
    // bit1-3 (exit mode) : 0b010 / On first Pairing success
    // bit4 (channel present) : 0b0 / shall always be set to 0 according current spec.
    // bit5 (unicast communication) : 0b0 / send GP Commissioning Notification commands in broadcast
    // bit6-7 (reserved)
    l_gp_comm_payload.push_back(0x05); 

    // comm windows 2 bytes
    // present only if exit mode flag On commissioning Window expiration (bit0) is set
    /*
    l_gp_comm_payload.push_back(u16_get_lo_u8(180));
    l_gp_comm_payload.push_back(u16_get_hi_u8(180));
    */
    // channel
    // never present with current specification
    /*
    l_gp_comm_payload.push_back(0);
    */

    // create message sending from ep242 to ep242 using green power profile
    l_gp_comm_msg.SetSpecific(GP_PROFILE_ID, PUBLIC_CODE, GP_ENDPOINT, 
                                GP_CLUSTER_ID, GP_PROXY_COMMISIONING_MODE_CLIENT_CMD_ID,
                                E_DIR_SERVER_TO_CLIENT, l_gp_comm_payload, 0, 0, 0);

    // WARNING use ep 242 as sources
    l_gp_comm_msg.aps.src_ep = GP_ENDPOINT;
    
    //
    clogI << "SEND UNICAST : OPEN GP COMMISSIONING\n";
    zb_messaging.SendUnicast(0,l_gp_comm_msg);
}