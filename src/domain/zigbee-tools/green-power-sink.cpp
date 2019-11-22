/**
 * @file green-power-sink.cpp
 *
 * @brief Access to green power capabilities
 */

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <map>

#include "green-power-sink.h"
#include "../ezsp-protocol/struct/ember-gp-address-struct.h"

#include "../byte-manip.h"

#include "../../domain/zbmessage/zigbee-message.h"
#include "../../domain/zbmessage/gpd-commissioning-command-payload.h"


#include "../../spi/GenericLogger.h"
#include "../../spi/ILogger.h"

// some defines to help understanding
#define GP_ENDPOINT 242

// cluster
#define GP_CLUSTER_ID   0x0021
// receive client command
#define GP_PROXY_COMMISIONING_MODE_CLIENT_CMD_ID    0x02

// GPF Command
#define GPF_SCENE_0_CMD		0x10
#define GPF_SCENE_1_CMD		0x11
#define GPF_SCENE_2_CMD		0x12
#define GPF_SCENE_3_CMD		0x13
#define GPF_SCENE_4_CMD		0x14
#define GPF_SCENE_5_CMD		0x15
#define GPF_SCENE_6_CMD		0x16
#define GPF_SCENE_7_CMD		0x17

#define GPF_STORE_SCENE_0_CMD		0x18
#define GPF_STORE_SCENE_1_CMD		0x19
#define GPF_STORE_SCENE_2_CMD		0x1A
#define GPF_STORE_SCENE_3_CMD		0x1B
#define GPF_STORE_SCENE_4_CMD		0x1C
#define GPF_STORE_SCENE_5_CMD		0x1D
#define GPF_STORE_SCENE_6_CMD		0x1E
#define GPF_STORE_SCENE_7_CMD		0x1F

#define GPF_OFF_CMD		0x20
#define GPF_ON_CMD		0x21
#define GPF_TOGGLE_CMD	0x22

#define GPF_UP_W_ON_OFF_CMD		0x34
#define GPF_STOP_CMD			0x35
#define GPF_DOWN_W_ON_OFF_CMD	0x36

#define GPF_COMMISSIONING_CMD	0xE0
#define GPF_DECOMMISSIONING_CMD	0xE1
#define GPF_CHANNEL_REQUEST_CMD	0xE3



CGpSink::CGpSink( CEzspDongle &i_dongle, CZigbeeMessaging &i_zb_messaging ) :
    dongle(i_dongle),
    zb_messaging(i_zb_messaging),
    sink_state(SINK_NOT_INIT),
    gpf_comm_frame(),
    sink_table_index(0xFF),
    gpds_to_register(),
    sink_table_entry(),
    observers()
{
    dongle.registerObserver(this);
}

void CGpSink::init()
{
    // initialize green power sink
    clogD << "Call EZSP_GP_SINK_TABLE_INIT" << std::endl;
    dongle.sendCommand(EZSP_GP_SINK_TABLE_INIT);

    // set state
    setSinkState(SINK_READY);    
}

void CGpSink::gpClearAllTables()
{
    // sink table
    dongle.sendCommand(EZSP_GP_SINK_TABLE_CLEAR_ALL); 
}

void CGpSink::openCommissioningSession()
{
    // set local proxy in commissioning mode
    sendLocalGPProxyCommissioningMode(0x05);

    // set state
    setSinkState(SINK_COM_OPEN);
}

void CGpSink::closeCommissioningSession()
{
    // set local proxy in commissioning mode
    sendLocalGPProxyCommissioningMode(0x00);

    // set state
    setSinkState(SINK_READY);
}

void CGpSink::registerGpds( const std::vector<CGpDevice> &gpd )
{
    // save offline information
    gpds_to_register = gpd;

    // request sink table entry
    gpSinkTableFindOrAllocateEntry( gpds_to_register.back().getSourceId() );
    
    // set state
    setSinkState(SINK_COM_OFFLINE_IN_PROGRESS);
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

            clogD << "EZSP_GPEP_INCOMING_MESSAGE_HANDLER status : " << CEzspEnum::EEmberStatusToString(l_status) <<
                ", link : " << unsigned(i_msg_receive.at(1)) <<
                ", sequence number : " << unsigned(i_msg_receive.at(2)) <<
                ", gp address : " << gpf <<
                std::endl;

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
                // do action only if we are in commissioning mode
                if( SINK_COM_OPEN == sink_state )
                {
                    if(  GPF_COMMISSIONING_CMD == gpf.getCommandId() )
                    {
                        // find entry in sink table
                        gpSinkTableFindOrAllocateEntry(gpf.getSourceId());

                        // save incomming message
                        gpf_comm_frame = gpf;

                        // set new state
                        setSinkState(SINK_COM_IN_PROGRESS);
                    }
                    else if( GPF_CHANNEL_REQUEST_CMD == gpf.getCommandId() )
                    {
                        // 
                    }
                }
            }
            else
            {
                // if success notify
                if(  EEmberStatus::EMBER_SUCCESS == l_status )
                {
                    notifyObserversOfRxGpFrame( gpf );
                }
            }
        }
        break;

        case EZSP_GP_SINK_TABLE_FIND_OR_ALLOCATE_ENTRY:
        {
            if( SINK_COM_IN_PROGRESS == sink_state )
            {
                // save allocate index
                sink_table_index = i_msg_receive.at(0);

                // debug
                clogD << "EZSP_GP_SINK_TABLE_FIND_OR_ALLOCATE_ENTRY response index : " << std::hex << std::setw(2) << std::setfill('0') << sink_table_index << std::endl;

                // retrieve entry at selected index
                if( 0xFF != sink_table_index )
                {
                    gpSinkGetEntry( sink_table_index );
                }
                else
                {
                    // no place to done pairing : FAILED
                    clogD << "INVALID SINK TABLE ENTRY, PAIRING FAILED !!" << std::endl;
                    setSinkState(SINK_READY);
                }
            }
            else if( SINK_COM_OFFLINE_IN_PROGRESS == sink_state )
            {
                // save allocate index
                sink_table_index = i_msg_receive.at(0);

                // debug
                clogD << "EZSP_GP_SINK_TABLE_FIND_OR_ALLOCATE_ENTRY response index : " << std::hex << std::setw(2) << std::setfill('0') << sink_table_index << std::endl;

                // retrieve entry at selected index
                if( 0xFF != sink_table_index )
                {
                    gpSinkGetEntry( sink_table_index );
                }
                else
                {
                    // no place to done pairing : FAILED
                    clogD << "INVALID SINK TABLE ENTRY, PAIRING FAILED !!" << std::endl;
                    setSinkState(SINK_READY);
                }
            }
        }
        break;

        case EZSP_GP_SINK_TABLE_GET_ENTRY:
        {
            if( SINK_COM_IN_PROGRESS == sink_state )
            {
                EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));
                CEmberGpSinkTableEntryStruct l_entry({i_msg_receive.begin()+1,i_msg_receive.end()});

                // debug
                clogD << "EZSP_GP_SINK_TABLE_GET_ENTRY Response status :" <<  CEzspEnum::EEmberStatusToString(l_status) << ", table entry : " << l_entry << std::endl;

                // decode payload
                CGpdCommissioningPayload l_payload(gpf_comm_frame.getPayload(),gpf_comm_frame.getSourceId());

                // debug
                clogD << "GPD Commissioning payload : " << l_payload << std::endl;

                // update sink table entry
                CEmberGpAddressStruct l_gpd_addr(gpf_comm_frame.getSourceId());
                CEmberGpSinkTableOption l_options(l_gpd_addr.getApplicationId(),l_payload);

                l_entry.setEntryActive(true);
                l_entry.setOptions(l_options);
                l_entry.setGpdAddress(l_gpd_addr);
                l_entry.setDeviceId(l_payload.getDeviceId());
                l_entry.setAlias(static_cast<uint16_t>(gpf_comm_frame.getSourceId()&0xFFFF));
                l_entry.setSecurityOption(l_payload.getExtendedOption()&0x1F);
                l_entry.setFrameCounter(l_payload.getOutFrameCounter());
                l_entry.setKey(l_payload.getKey());

                // debug
                clogD << "Update table entry : " << l_entry << std::endl;

                // call
                gpSinkSetEntry(sink_table_index,l_entry);

                // save
                sink_table_entry = l_entry;
            }
            else if( SINK_COM_OFFLINE_IN_PROGRESS == sink_state )
            {
                EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));
                CEmberGpSinkTableEntryStruct l_entry({i_msg_receive.begin()+1,i_msg_receive.end()});

                // debug
                clogD << "EZSP_GP_SINK_TABLE_GET_ENTRY Response status :" <<  CEzspEnum::EEmberStatusToString(l_status) << ", table entry : " << l_entry << std::endl;

                // update sink table entry
                CEmberGpAddressStruct l_gp_addr(gpds_to_register.back().getSourceId());

                l_entry.setEntryActive(true);
                l_entry.setOptions(gpds_to_register.back().getSinkOption());
                l_entry.setGpdAddress(l_gp_addr);
                l_entry.setAlias(static_cast<uint16_t>(l_gp_addr.getSourceId()&0xFFFF));
                l_entry.setSecurityOption(gpds_to_register.back().getSinkSecurityOption());
                l_entry.setFrameCounter(0);
                l_entry.setKey(gpds_to_register.back().getKey());

                // debug
                clogD << "Update table entry : " << l_entry << std::endl;

                // call
                gpSinkSetEntry(sink_table_index,l_entry);

                // save
                sink_table_entry = l_entry;
            }
        }
        break;

        case EZSP_GP_SINK_TABLE_SET_ENTRY:
        {
            if( (SINK_COM_IN_PROGRESS == sink_state) || (SINK_COM_OFFLINE_IN_PROGRESS == sink_state) )
            {
                EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));

                // debug
                clogD << "EZSP_GP_SINK_TABLE_SET_ENTRY Response status :" <<  CEzspEnum::EEmberStatusToString(l_status) << std::endl;

                if( EMBER_SUCCESS != l_status )
                {
                    // error
                    clogD << "ERROR, Stop commissioning process !!" << std::endl;
                    setSinkState(SINK_READY);
                }
                else
                {
                    // do proxy pairing
                    // \todo replace short and long sink network address by right value, currently we use group mode not so important
                    CProcessGpPairingParam l_param( sink_table_entry, true, false, 0, {0,0,0,0,0,0,0,0} );
                    // call
                    gpProxyTableProcessGpPairing(l_param);  
                }              
            }
        }
        break;

        case EZSP_GP_PROXY_TABLE_PROCESS_GP_PAIRING:
        {
            if( SINK_COM_IN_PROGRESS == sink_state )
            {
                clogI << "CGpSink::ezspHandler EZSP_GP_PROXY_TABLE_PROCESS_GP_PAIRING gpPairingAdded : " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[0]) << std::endl;

                // close commissioning session
                closeCommissioningSession();
            }
            else if( SINK_COM_OFFLINE_IN_PROGRESS == sink_state )
            {
                clogI << "CGpSink::ezspHandler EZSP_GP_PROXY_TABLE_PROCESS_GP_PAIRING gpPairingAdded : " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[0]) << std::endl;

                gpds_to_register.pop_back();

                if( gpds_to_register.size() )
                {
                    // request sink table entry
                    gpSinkTableFindOrAllocateEntry( gpds_to_register.back().getSourceId() );
                }
                else
                {
                    // set state
                    setSinkState(SINK_READY);
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

void CGpSink::sendLocalGPProxyCommissioningMode(uint8_t i_option)
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
    l_gp_comm_payload.push_back(i_option); // 0x05 => open

    // comm windows 2 bytes
    // present only if exit mode flag On commissioning Window expiration (bit0) is set

    // channel 1 byte
    // never present with current specification


    // create message sending from ep242 to ep242 using green power profile
    l_gp_comm_msg.SetSpecific(GP_PROFILE_ID, PUBLIC_CODE, GP_ENDPOINT, 
                                GP_CLUSTER_ID, GP_PROXY_COMMISIONING_MODE_CLIENT_CMD_ID,
                                E_DIR_SERVER_TO_CLIENT, l_gp_comm_payload, 0, 0, 0);

    // WARNING use ep 242 as sources
    l_gp_comm_msg.aps.src_ep = GP_ENDPOINT;
    
    //
    clogI << "SEND UNICAST : OPEN/CLOSE GP COMMISSIONING option : " <<  std::hex << std::setw(2) << std::setfill('0') << i_option << std::endl;
    zb_messaging.SendUnicast(0,l_gp_comm_msg);
}

/*
void CGpSink::gpBrCommissioningNotification( uint32_t i_gpd_src_id, uint8_t i_seq_number )
{
    std::vector<uint8_t> l_proxy_br_payload;

    // The source from which to send the broadcast
    l_proxy_br_payload.push_back(static_cast<uint8_t>(i_gpd_src_id&0xFF));
    l_proxy_br_payload.push_back(static_cast<uint8_t>((i_gpd_src_id>>8)&0xFF));

    // The destination to which to send the broadcast. This must be one of the three ZigBee broadcast addresses.
    l_proxy_br_payload.push_back(0xFD);
    l_proxy_br_payload.push_back(0xFF);

    // The network sequence number for the broadcast
    l_proxy_br_payload.push_back(i_seq_number);

    // The APS frame for the message.
    CAPSFrame l_aps;
    l_aps.SetDefaultAPS(GP_PROFILE_ID,GP_CLUSTER_ID,GP_ENDPOINT);
    l_aps.src_ep = GP_ENDPOINT;
    std::vector<uint8_t> l_ember_aps = l_aps.GetEmberAPS();
    l_proxy_br_payload.insert(l_proxy_br_payload.end(), l_ember_aps.begin(), l_ember_aps.end());

    // The message will be delivered to all nodes within radius hops of the sender. A radius of zero is converted to EMBER_MAX_HOPS.
    l_proxy_br_payload.push_back(0x00);

    // A value chosen by the Host. This value is used in the ezspMessageSentHandler response to refer to this message.
    l_proxy_br_payload.push_back(0x00);

    // The length of the messageContents parameter in bytes.
    l_proxy_br_payload.push_back(49);

    // The broadcast message.
    l_proxy_br_payload.push_back(0x11);
    l_proxy_br_payload.push_back(0x06);
    l_proxy_br_payload.push_back(0x04);
    l_proxy_br_payload.push_back(0x00);
    l_proxy_br_payload.push_back(0x08);
    l_proxy_br_payload.push_back(0x50);
    l_proxy_br_payload.push_back(0x00);
    l_proxy_br_payload.push_back(0x51);
    l_proxy_br_payload.push_back(0x00);
    l_proxy_br_payload.push_back(0x24);
    l_proxy_br_payload.push_back(0x00);
    l_proxy_br_payload.push_back(0x00);
    l_proxy_br_payload.push_back(0x00);
    l_proxy_br_payload.push_back(0xe0);
    l_proxy_br_payload.push_back(0x1f);
    l_proxy_br_payload.push_back(0x02);
    l_proxy_br_payload.push_back(0xc5);
    l_proxy_br_payload.push_back(0xf2);
    l_proxy_br_payload.push_back(0xa8);
    l_proxy_br_payload.push_back(0xac);
    l_proxy_br_payload.push_back(0x43);
    l_proxy_br_payload.push_back(0x76);
    l_proxy_br_payload.push_back(0x30);
    l_proxy_br_payload.push_back(0x80);
    l_proxy_br_payload.push_back(0x89);
    l_proxy_br_payload.push_back(0x5f);
    l_proxy_br_payload.push_back(0x3c);
    l_proxy_br_payload.push_back(0xd5);
    l_proxy_br_payload.push_back(0xdc);
    l_proxy_br_payload.push_back(0x9a);
    l_proxy_br_payload.push_back(0xd8);
    l_proxy_br_payload.push_back(0x87);
    l_proxy_br_payload.push_back(0x1c);
    l_proxy_br_payload.push_back(0x0d);
    l_proxy_br_payload.push_back(0x15);
    l_proxy_br_payload.push_back(0xde);
    l_proxy_br_payload.push_back(0x17);
    l_proxy_br_payload.push_back(0x2b);
    l_proxy_br_payload.push_back(0x24);
    l_proxy_br_payload.push_back(0x00);
    l_proxy_br_payload.push_back(0x00);
    l_proxy_br_payload.push_back(0x00);
    l_proxy_br_payload.push_back(0x04);
    l_proxy_br_payload.push_back(0x02);
    l_proxy_br_payload.push_back(0x20);
    l_proxy_br_payload.push_back(0x21);
    l_proxy_br_payload.push_back(0x00);
    l_proxy_br_payload.push_back(0x00);
    l_proxy_br_payload.push_back(0xdc);

    clogI << "EZSP_PROXY_BROADCAST\n";
    dongle.sendCommand(EZSP_PROXY_BROADCAST,l_proxy_br_payload);
}
*/
/*
void CAppDemo::ezspGetExtendedValue( uint8_t i_value_id, uint32_t i_characteristic )
{
    std::vector<uint8_t> l_payload;

    // Identifies which extended value ID to read.
    l_payload.push_back(i_value_id);

    // Identifies which characteristics of the extended value ID to read. These are specific to the value being read.
    l_payload.push_back((uint8_t)(i_characteristic&0xFF));
    l_payload.push_back((uint8_t)((i_characteristic>>8)&0xFF));
    l_payload.push_back((uint8_t)((i_characteristic>>16)&0xFF));
    l_payload.push_back((uint8_t)((i_characteristic>>24)&0xFF));

    clogI << "EZSP_GET_EXTENDED_VALUE\n";
    dongle.sendCommand(EZSP_GET_EXTENDED_VALUE,l_payload);
}
*/

/*
void CAppDemo::gpSinkTableLookup( uint32_t i_src_id )
{
    std::vector<uint8_t> l_payload;

    // EmberGpAddress addr The address to search for.
    l_payload.push_back(0x00);
    l_payload.push_back((uint8_t)(i_src_id&0xFF));
    l_payload.push_back((uint8_t)((i_src_id>>8)&0xFF));
    l_payload.push_back((uint8_t)((i_src_id>>16)&0xFF));
    l_payload.push_back((uint8_t)((i_src_id>>24)&0xFF));
    l_payload.push_back((uint8_t)(i_src_id&0xFF));
    l_payload.push_back((uint8_t)((i_src_id>>8)&0xFF));
    l_payload.push_back((uint8_t)((i_src_id>>16)&0xFF));
    l_payload.push_back((uint8_t)((i_src_id>>24)&0xFF));
    l_payload.push_back(0x00);

    clogI << "EZSP_GP_SINK_TABLE_LOOKUP\n";
    dongle.sendCommand(EZSP_GP_SINK_TABLE_LOOKUP,l_payload);    
}
*/

void CGpSink::gpSinkTableFindOrAllocateEntry( uint32_t i_src_id )
{
    // An EmberGpAddress struct containing a copy of the gpd address to be found.
    CEmberGpAddressStruct l_gp_address(i_src_id);

    clogI << "EZSP_GP_SINK_TABLE_FIND_OR_ALLOCATE_ENTRY\n";
    dongle.sendCommand(EZSP_GP_SINK_TABLE_FIND_OR_ALLOCATE_ENTRY,l_gp_address.getRaw());    
}

void CGpSink::gpSinkGetEntry( uint8_t i_index )
{
    std::vector<uint8_t> l_payload;

    // The index of the requested sink table entry.
    l_payload.push_back(i_index);

    clogI << "EZSP_GP_SINK_TABLE_GET_ENTRY\n";
    dongle.sendCommand(EZSP_GP_SINK_TABLE_GET_ENTRY,l_payload);    
}


void CGpSink::gpSinkSetEntry( uint8_t i_index, CEmberGpSinkTableEntryStruct& i_entry )
{
    std::vector<uint8_t> l_payload;

    // The index of the requested sink table entry.
    l_payload.push_back(i_index);

    // struct
    std::vector<uint8_t> i_struct = i_entry.getRaw();
    l_payload.insert(l_payload.end(), i_struct.begin(), i_struct.end());

    clogI << "EZSP_GP_SINK_TABLE_SET_ENTRY\n";
    dongle.sendCommand(EZSP_GP_SINK_TABLE_SET_ENTRY,l_payload);    
}


void CGpSink::gpProxyTableProcessGpPairing( CProcessGpPairingParam& i_param )
{
    clogI << "EZSP_GP_PROXY_TABLE_PROCESS_GP_PAIRING\n";
    dongle.sendCommand(EZSP_GP_PROXY_TABLE_PROCESS_GP_PAIRING,i_param.get());    
}


void CGpSink::setSinkState( ESinkState i_state )
{
    sink_state = i_state;

    const std::map<ESinkState,std::string> MyEnumStrings {
        { SINK_NOT_INIT, "SINK_NOT_INIT" },
        { SINK_READY, "SINK_READY" },
        { SINK_ERROR, "SINK_ERROR" },
        { SINK_COM_OPEN, "SINK_COM_OPEN" },
        { SINK_COM_IN_PROGRESS, "SINK_COM_IN_PROGRESS" },
        { SINK_COM_OFFLINE_IN_PROGRESS, "SINK_COM_OFFLINE_IN_PROGRESS" },
    };

    auto  it  = MyEnumStrings.find(sink_state); /* FIXME: we issue a warning, but the variable app_state is now out of bounds */
    std::string error_str = it == MyEnumStrings.end() ? "OUT_OF_RANGE" : it->second;
    clogI << "SINK State change : " << error_str << std::endl;
}
