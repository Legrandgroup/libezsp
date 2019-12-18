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
#include <string>

#include "ezsp/zigbee-tools/green-power-sink.h"
#include "ezsp/ezsp-protocol/struct/ember-gp-address-struct.h"
#include "ezsp/ezsp-protocol/struct/ember-gp-proxy-table-entry-struct.h"

#include "ezsp/byte-manip.h"

#include "ezsp/zbmessage/zigbee-message.h"
#include "ezsp/zbmessage/gpd-commissioning-command-payload.h"
#include "ezsp/ezsp-protocol/get-network-parameters-response.h"

#include "spi/ILogger.h"

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

#define GPF_MANUFACTURER_ATTRIBUTE_REPORTING 0xA1

#define GPF_COMMISSIONING_CMD	0xE0
#define GPF_DECOMMISSIONING_CMD	0xE1
#define GPF_CHANNEL_REQUEST_CMD	0xE3

// GPDF commands sent to GPD
#define GPF_CHANNEL_CONFIGURATION   0xF3

// MSP GPF
#define GPF_MSP_CHANNEL_REQUEST_CMD	0xB0



CGpSink::CGpSink( CEzspDongle &i_dongle, CZigbeeMessaging &i_zb_messaging ) :
    dongle(i_dongle),
    zb_messaging(i_zb_messaging),
    sink_state(SINK_NOT_INIT),
    nwk_parameters(),
    authorizeGpfChannelRqst(false),
    gpf_comm_frame(),
    sink_table_index(0xFF),
    gpds_to_register(),
    sink_table_entry(),
    proxy_table_index(),
    gpds_to_remove(),
    gpd_send_list(),
    observers()
{
    dongle.registerObserver(this);
}

void CGpSink::init()
{
    // initialize green power sink
    clogD << "Call EZSP_GP_SINK_TABLE_INIT" << std::endl;
    dongle.sendCommand(EZSP_GP_SINK_TABLE_INIT);

    // retieve network information
    dongle.sendCommand(EZSP_GET_NETWORK_PARAMETERS);

    // set state
    setSinkState(SINK_READY);
}

bool CGpSink::gpClearAllTables()
{
    bool lo_success = false;

    if( SINK_READY == sink_state )
    {
        // sink table
        dongle.sendCommand(EZSP_GP_SINK_TABLE_CLEAR_ALL);

        // proxy table
        proxy_table_index = 0;
        dongle.sendCommand(EZSP_GP_PROXY_TABLE_GET_ENTRY,{proxy_table_index});

        // set state
        setSinkState(SINK_CLEAR_ALL);
        lo_success = true;
    }
    return lo_success;
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

void CGpSink::removeGpds( const std::vector<uint32_t> &gpd )
{
    // save offline information
    gpds_to_remove = gpd;

    // request sink table entry
    gpSinkTableLookup( gpds_to_remove.back() );

    // set state
    setSinkState(SINK_REMOVE_IN_PROGRESS);
}

void CGpSink::handleDongleState( EDongleState i_state )
{
}

void CGpSink::handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive )
{
    switch( i_cmd )
    {
        case EZSP_GP_PROXY_TABLE_GET_ENTRY:
        {
            if( SINK_CLEAR_ALL == sink_state )
            {
                EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));
                if( EMBER_SUCCESS == l_status )
                {
                    // do remove action
                    CEmberGpProxyTableEntryStruct l_entry(std::vector<uint8_t>(i_msg_receive.begin()+1,i_msg_receive.end()));
                    CProcessGpPairingParam l_param(l_entry.getGpdAddress().getSourceId());
                    gpProxyTableProcessGpPairing(l_param);
                }
                else
                {
                    // assume end of table
                    // set new state
                    setSinkState(SINK_READY);
                }

            }
        }
        break;
        case EZSP_GET_NETWORK_PARAMETERS:
        {
            CGetNetworkParamtersResponse l_rsp(i_msg_receive);
            if( EEmberStatus::EMBER_SUCCESS == l_rsp.getStatus() )
            {
                nwk_parameters = l_rsp.getParameters();
            }
        }
        break;
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

            // notify
            CGpdKeyStatus l_key_status = CGpdKeyStatus::Undefined;
            if( EEmberStatus::EMBER_SUCCESS == l_status ){ l_key_status = CGpdKeyStatus::Valid; }
            else if( 0x7E == l_status ){ l_key_status = CGpdKeyStatus::Invalid; }
            else{ l_key_status = CGpdKeyStatus::Undefined; }
            notifyObserversOfRxGpdId(gpf.getSourceId(), (gpf.getProxyTableEntry()!=0xFF?true:false), l_key_status);

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
                }
                if( authorizeGpfChannelRqst && (GPF_CHANNEL_REQUEST_CMD == gpf.getCommandId()) )
                {
                    // response only if next attempt is on same channel as us
                    uint8_t l_next_channel_attempt = static_cast<uint8_t>(gpf.getPayload().at(0)&0x0F);
                    if( l_next_channel_attempt == (nwk_parameters.getRadioChannel()-11U) )
                    {
                        // send hannel configuration with timeout of 500ms
                        CEmberGpAddressStruct l_gp_addr(gpf.getSourceId());
                        std::vector<uint8_t> l_payload;
                        l_payload.push_back(0x10|l_next_channel_attempt);
                        gpSend(true, true, l_gp_addr, GPF_CHANNEL_CONFIGURATION,l_payload, 2000 );

                        // \todo is it necessary to let SINK open for commissioning ?
                    }
                }
            }
            else
            {
                // if success notify
                if(  EEmberStatus::EMBER_SUCCESS == l_status )
                {
                    // manage channel request
                    if( GPF_MANUFACTURER_ATTRIBUTE_REPORTING == gpf.getCommandId() )
                    {
                        // assume manufacturing 0x1021 attribute 0x5000 of cluster 0x0000 is a secure channel request
                        uint16_t l_manufacturer_id = dble_u8_to_u16(gpf.getPayload().at(1), gpf.getPayload().at(0));
                        if( 0x1021 == l_manufacturer_id )
                        {
                            uint16_t l_cluster_id = dble_u8_to_u16(gpf.getPayload().at(3), gpf.getPayload().at(2));
                            uint16_t l_attribute_id = dble_u8_to_u16(gpf.getPayload().at(5), gpf.getPayload().at(4));
                            uint8_t l_type_id = gpf.getPayload().at(6);
                            //uint8_t l_device_id = gpf.getPayload().at(7);	// Unused for now

                            if( (0==l_cluster_id) && (0x5000==l_attribute_id) && (0x20==l_type_id) )
                            {
                                // verify that no message is waiting to send
                                bool l_found = false;
                                for (auto it : gpd_send_list)
                                    if( it.second == gpf.getSourceId() ){ l_found = true; }

                                if( !l_found )
                                {
                                    static uint8_t l_handle_counter = 0;

                                    // response on same channel, attribute contain device_id of gpd
                                    // \todo use to update sink table entry

                                    // send hannel configuration with timeout of 1000ms
                                    CEmberGpAddressStruct l_gp_addr(gpf.getSourceId());
                                    std::vector<uint8_t> l_payload;
                                    l_payload.push_back(static_cast<uint8_t>(0x10|((nwk_parameters.getRadioChannel()-11U)&0x0F)));
                                    gpSend( true, true, l_gp_addr, GPF_CHANNEL_CONFIGURATION,l_payload, 1000, l_handle_counter );
                                    gpd_send_list.insert({l_handle_counter++,gpf.getSourceId()});
                                }
                            }
                        }
                    }

                    // notify
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

        case EZSP_GP_SINK_TABLE_LOOKUP:
        {
            if ( SINK_REMOVE_IN_PROGRESS == sink_state )
            {
                if( 0xFF != i_msg_receive.at(0) )
                {
                    // remove index
                    gpSinkTableRemoveEntry(i_msg_receive.at(0));
                }

                // find proxy table entry
                gpProxyTableLookup(gpds_to_remove.back());
            }
        }
        break;

        case EZSP_GP_PROXY_TABLE_LOOKUP:
        {
            if ( SINK_REMOVE_IN_PROGRESS == sink_state )
            {
                if( 0xFF != i_msg_receive.at(0) )
                {
                    // remove index
                    CProcessGpPairingParam l_param(gpds_to_remove.back());
                    gpProxyTableProcessGpPairing(l_param);
                }

                // find next sink table entry
                gpds_to_remove.pop_back();
                if( gpds_to_remove.empty() )
                {
                    // no more gpd to remove
                    setSinkState(SINK_READY);
                }
                else
                {
                    gpSinkTableLookup(gpds_to_remove.back());
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
            else if( SINK_CLEAR_ALL == sink_state )
            {
                // retrieve next entry
                proxy_table_index++;
                dongle.sendCommand(EZSP_GP_PROXY_TABLE_GET_ENTRY,{proxy_table_index});
            }
        }
        break;

        case EZSP_D_GP_SEND:
        {
            EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));

            // debug
            clogD << "EZSP_D_GP_SEND Response status :" <<  CEzspEnum::EEmberStatusToString(l_status) << std::endl;
        }
        break;

        case EZSP_D_GP_SENT_HANDLER:
        {
            EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));

            if( EMBER_SUCCESS == l_status )
                gpd_send_list.erase(i_msg_receive.at(1));

            // debug
            clogD << "EZSP_D_GP_SENT_HANDLER Response status :" <<  CEzspEnum::EEmberStatusToString(l_status) << std::endl;
        }
        break;

        case EZSP_SEND_RAW_MESSAGE:
        {
            EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));

            // debug
            clogD << "EZSP_SEND_RAW_MESSAGE Response status :" <<  CEzspEnum::EEmberStatusToString(l_status) << std::endl;
        }
        break;

        case EZSP_RAW_TRANSMIT_COMPLETE_HANDLER:
        {
            EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));

            // debug
            clogD << "EZSP_RAW_TRANSMIT_COMPLETE_HANDLER Response status :" <<  CEzspEnum::EEmberStatusToString(l_status) << std::endl;
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

void CGpSink::notifyObserversOfRxGpdId( uint32_t i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status ) {
    for(auto observer : this->observers) {
        observer->handleRxGpdId( i_gpd_id, i_gpd_known, i_gpd_key_status );
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

void CGpSink::gpSend(bool i_action, bool i_use_cca, CEmberGpAddressStruct i_gp_addr,
                uint8_t i_gpd_command_id, std::vector<uint8_t> i_gpd_command_payload, uint16_t i_life_time_ms, uint8_t i_handle )
{
    std::vector<uint8_t> l_payload;

    // The action to perform on the GP TX queue (true to add, false to remove).
    l_payload.push_back(i_action);
    // Whether to use ClearChannelAssessment when transmitting the GPDF.
    l_payload.push_back(i_use_cca);
    // The Address of the destination GPD.
    std::vector<uint8_t> i_addr = i_gp_addr.getRaw();
    l_payload.insert(l_payload.end(), i_addr.begin(), i_addr.end());
    // The GPD command ID to send.
    l_payload.push_back(i_gpd_command_id);
    // The length of the GP command payload.
    std::string::size_type payload_size = i_gpd_command_payload.size();
    if (payload_size > static_cast<uint8_t>(-1)) {
        clogE << "Payload size overflow: " << payload_size << ", truncating to a 255\n";
        payload_size = static_cast<uint8_t>(-1);
    }
    l_payload.push_back(static_cast<uint8_t>(payload_size));
    // The GP command payload.
    l_payload.insert(l_payload.end(), i_gpd_command_payload.begin(), i_gpd_command_payload.end());
    // The handle to refer to the GPDF.
    l_payload.push_back(i_handle);
    // How long to keep the GPDF in the TX Queue.
    l_payload.push_back(static_cast<uint8_t>(i_life_time_ms&0xFF));
    l_payload.push_back(static_cast<uint8_t>((i_life_time_ms>>8)&0xFF));

    clogI << "EZSP_D_GP_SEND\n";
    dongle.sendCommand(EZSP_D_GP_SEND,l_payload);
}

void CGpSink::gpSinkTableRemoveEntry( uint8_t i_index )
{
    clogI << "EZSP_GP_SINK_TABLE_REMOVE_ENTRY\n";
    dongle.sendCommand(EZSP_GP_SINK_TABLE_REMOVE_ENTRY,{i_index});
}

void CGpSink::gpProxyTableLookup(uint32_t i_src_id)
{
    CEmberGpAddressStruct i_addr(i_src_id);
    clogI << "EZSP_GP_PROXY_TABLE_LOOKUP\n";
    dongle.sendCommand(EZSP_GP_PROXY_TABLE_LOOKUP,i_addr.getRaw());
}

void CGpSink::gpSinkTableLookup(uint32_t i_src_id)
{
    CEmberGpAddressStruct i_addr(i_src_id);
    clogI << "EZSP_GP_SINK_TABLE_LOOKUP\n";
    dongle.sendCommand(EZSP_GP_SINK_TABLE_LOOKUP,i_addr.getRaw());
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
        { SINK_AUTHORIZE_ANSWER_CH_RQST, "SINK_AUTHORIZE_ANSWER_CH_RQST" },
        { SINK_CLEAR_ALL, "SINK_CLEAR_ALL" },
        { SINK_REMOVE_IN_PROGRESS, "SINK_REMOVE_IN_PROGRESS" },
    };

    auto  it  = MyEnumStrings.find(sink_state); /* FIXME: we issue a warning, but the variable app_state is now out of bounds */
    std::string error_str = it == MyEnumStrings.end() ? "OUT_OF_RANGE" : it->second;
    clogI << "SINK State change : " << error_str << std::endl;
}
