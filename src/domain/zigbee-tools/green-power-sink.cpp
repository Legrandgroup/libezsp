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

#include "../../spi/GenericLogger.h"
#include "../../spi/ILogger.h"



CGpSink::CGpSink( CEzspDongle &i_dongle ) :
    dongle(i_dongle),
    sink_table(),
    observers()
{
    dongle.registerObserver(this);
}

uint8_t CGpSink::registerGpd( uint32_t i_source_id )
{
    CGpSinkTableEntry l_entry = CGpSinkTableEntry(i_source_id);

    return sink_table.addEntry(l_entry);
}


void CGpSink::sendGPF(uint32_t i_src_id, uint8_t i_cmd, std::vector<uint8_t> i_msg)
{
    std::vector<uint8_t> l_gpf;

    /* The action to perform on the GP TX queue (true to add, false to remove). */
    l_gpf.push_back(0x01);

    /* Whether to use ClearChannelAssessment when transmitting the GPDF. */
    l_gpf.push_back(0x01);

    /* The Address of the destination GPD. */
    // GPD Source Id (4 bytes) + 4 for fill ieeee
    l_gpf.push_back(static_cast<uint8_t>(i_src_id & 0x000000FF));
    l_gpf.push_back(static_cast<uint8_t>((i_src_id >> 8) & 0x000000FF));
    l_gpf.push_back(static_cast<uint8_t>((i_src_id >> 16) & 0x000000FF));
    l_gpf.push_back(static_cast<uint8_t>((i_src_id >> 24) & 0x000000FF));
    l_gpf.push_back(static_cast<uint8_t>(i_src_id & 0x000000FF));
    l_gpf.push_back(static_cast<uint8_t>((i_src_id >> 8) & 0x000000FF));
    l_gpf.push_back(static_cast<uint8_t>((i_src_id >> 16) & 0x000000FF));
    l_gpf.push_back(static_cast<uint8_t>((i_src_id >> 24) & 0x000000FF));
    // application Id
    l_gpf.push_back(0x00);
    // endpoint
    l_gpf.push_back(0);

    /* The GPD command ID to send. */
    l_gpf.push_back(i_cmd);

    if (i_msg.size()>0xff)
    {
        clogE << "Message size overflow\n";
    }

    /* The length of the GP command payload. */
    l_gpf.push_back(static_cast<uint8_t>(i_msg.size()));

    /* The GP command payload. */
    for( const auto &l_byte : i_msg )
    {
        l_gpf.push_back(l_byte);
    }

    /* The handle to refer to the GPDF. */
    l_gpf.push_back(0);

    /* How long to keep the GPDF in the TX Queue. */
    l_gpf.push_back(200);

/* DEBUG
    // 802.15.4 Header
    /// frame control
    l_gpf.push_back(0x01);
    l_gpf.push_back(0x08);
    /// sequence number : use last byte of frame control, shall be increment
    l_gpf.push_back(0x00);
    /// destination PAN
    l_gpf.push_back(0xff);
    l_gpf.push_back(0xff);
    /// destination
    l_gpf.push_back(0xff);
    l_gpf.push_back(0xff);

    // GPF NWK Header
    l_gpf.push_back(0x8C); // frame type data, protocol version 3, no auto commissioning, have nwk frame extension

    // GPF NWK Extend
    l_gpf.push_back(0xf0); // use srcId, Rx capable, security MIC Only, direction to gpd

    // GPD Source Id (4 bytes)
    l_gpf.push_back(static_cast<uint8_t>(i_src_id && 0xFF));
    l_gpf.push_back(static_cast<uint8_t>((i_src_id >> 8)&& 0xFF));
    l_gpf.push_back(static_cast<uint8_t>((i_src_id >> 16)&& 0xFF));
    l_gpf.push_back(static_cast<uint8_t>((i_src_id >> 24)&& 0xFF));

    // GPF Security frame counter (4 bytes) : TODO A FAIRE !!
    l_gpf.push_back(0);
    l_gpf.push_back(0);
    l_gpf.push_back(0);
    l_gpf.push_back(0);

    // GPF Application payload : command + payload
    l_gpf.push_back(i_cmd);
    for( const auto &l_byte : i_msg )
    {
        l_gpf.push_back(l_byte);
    }
    
    // GPF MIC (4 bytes) TODO Shall be implement !!
    l_gpf.push_back(0xff);
    l_gpf.push_back(0xff);
    l_gpf.push_back(0xff);
    l_gpf.push_back(0xff);
*/    

    dongle.sendCommand(EZSP_D_GP_SEND, l_gpf);
}

void CGpSink::handleDongleState( EDongleState i_state )
{
}

void CGpSink::handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive )
{
    switch( i_cmd )
    {
        case EZSP_D_GP_SEND:
        {
            EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));
            clogI << "EZSP_D_GP_SEND status : " << CEzspEnum::EEmberStatusToString(l_status) << std::endl;
        }
        break;

        case EZSP_D_GP_SENT_HANDLER:
        {
            EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));
            clogI << "EZSP_D_GP_SENT_HANDLER status : " << CEzspEnum::EEmberStatusToString(l_status) << std::endl;
        }
        break;

        case EZSP_GPEP_INCOMING_MESSAGE_HANDLER:
        {
            EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));

            // build gpf frame from ezsp rx message
            CGpFrame gpf = CGpFrame(i_msg_receive);


            // Start DEBUG
            clogI << "EZSP_GPEP_INCOMING_MESSAGE_HANDLER status : " << CEzspEnum::EEmberStatusToString(l_status) <<
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
            clogI << "CGpSink::ezspHandler cmd: " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_cmd) << ", msg: " << bufDump.str() << std::endl;
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
