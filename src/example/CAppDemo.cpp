/**
 * @file CAppDemo.cpp
 */

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <map>

#include "CAppDemo.h"
#include "../domain/ezsp-protocol/get-network-parameters-response.h"
#include "../domain/ezsp-protocol/struct/ember-key-struct.h"
#include "../domain/ezsp-protocol/ezsp-enum.h"
#include "../domain/zbmessage/zdp-enum.h"
#include "../domain/zbmessage/zigbee-message.h"
#include "../domain/byte-manip.h"


CAppDemo::CAppDemo(IUartDriver& uartDriver, ITimerFactory &i_timer_factory, bool reset, unsigned int networkChannel, const std::vector<uint32_t>& sourceIdList) :
    dongle(i_timer_factory, this),
    zb_messaging(dongle, i_timer_factory),
    zb_nwk(dongle, zb_messaging),
    gp_sink(dongle),
    app_state(APP_NOT_INIT),
    db(),
    ezsp_version(6),
    reset_wanted(reset),
    channel(networkChannel)
{
    setAppState(APP_NOT_INIT);
    // uart
    if (channel<11 || channel>27) {
        clogE << "Invalid channel: " << channel << ". Using 11 instead\n";
        channel = 11;
    }
    if( dongle.open(&uartDriver) ) {
        clogI << "CAppDemo open success !" << std::endl;
        dongle.registerObserver(this);
        gp_sink.registerObserver(this);
        for (auto i : sourceIdList) {
            clogD << "Watching source ID 0x" << std::hex << std::setw(8) << std::setfill('0') << i << "\n";
            gp_sink.registerGpd(i);
        }
        setAppState(APP_INIT_IN_PROGRESS);
    }
    // save parameter
    reset_wanted = reset;
}

void CAppDemo::handleDongleState( EDongleState i_state )
{
    clogI << "CAppDemo::dongleState : " << i_state << std::endl;

    if( DONGLE_READY == i_state )
    {
        if( APP_INIT_IN_PROGRESS == app_state )
        {
            dongleInit();
        }
    }
    else if( DONGLE_REMOVE == i_state )
    {
        // TODO: manage this !
    }
}

bool CAppDemo::extractClusterReport( const std::vector<uint8_t >& payload, size_t& usedBytes )
{
    size_t payloadSize = payload.size();

    if (payloadSize < 5)
    {
        clogE << "Attribute reporting frame is too short: " << payloadSize << " bytes\n";
        return false;
    }

    uint16_t clusterId = dble_u8_to_u16(payload.at(1), payload.at(0));
    uint16_t attributeId = dble_u8_to_u16(payload.at(3), payload.at(2));
    uint8_t type = payload.at(4);

    switch (clusterId)
    {
        case 0x000F: /* Binary input */
            if ((attributeId == 0x0055) && (type == ZCL_BOOLEAN_ATTRIBUTE_TYPE))
            {
                if (payloadSize < 6)
                {
                    clogE << "Binary input frame is too short: " << payloadSize << " bytes\n";
                    return false;
                }
                else
                {
                    uint8_t value = payload.at(5);
                    std::cout << "Door is " << (value?"closed":"open") << "\n";
                    usedBytes = 6;
                    return true;
                }
            }
            else
            {
                clogE << "Wrong type: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(type) << "\n";
                return false;
            }
            break;
        case 0x0402: /* Temperature */
            if ((attributeId == 0x0000) && (type == ZCL_INT16S_ATTRIBUTE_TYPE))
            {
                if (payloadSize < 7)
                {
                    clogE << "Temperature frame is too short: " << payloadSize << " bytes\n";
                    return false;
                }
                else
                {
                    int16_t value = static_cast<int16_t>(dble_u8_to_u16(payload.at(6), payload.at(5)));
                    std::cout << "Temperature: " << value/100 << "." << std::setw(2) << std::setfill('0') << value%100 << "°C\n";
                    usedBytes = 7;
                    return true;
                }
            }
            else
            {
                clogE << "Wrong type: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(type) << "\n";
                return false;
            }
            break;
        case 0x0405: /* Humidity */
            if ((attributeId == 0x0000) && (type == ZCL_INT16U_ATTRIBUTE_TYPE))
            {
                if (payloadSize < 7)
                {
                    clogE << "Humidity frame is too short: " << payloadSize << " bytes\n";
                    return false;
                }
                else
                {
                    int16_t value = static_cast<int16_t>(dble_u8_to_u16(payload.at(6), payload.at(5)));
                    std::cout << "Humidity: " << value/100 << "." << std::setw(2) << std::setfill('0') << value%100 << "%\n";
                    usedBytes = 7;
                    return true;
                }
            }
            else
            {
                clogE << "Wrong type: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(type) << "\n";
                return false;
            }
            break;
        case 0x0001: /* Battery level */
            if ((attributeId == 0x0020) && (type == ZCL_INT8U_ATTRIBUTE_TYPE))
            {
                if (payloadSize < 6)
                {
                    clogE << "Battery level frame is too short: " << payloadSize << " bytes\n";
                    return false;
                }
                else
                {
                    uint8_t value = static_cast<uint8_t>(payload.at(5));
                    std::cout << "Battery level: " << value/10 << "." << std::setw(1) << std::setfill('0') << value%10 << "V\n";
                    usedBytes = 6;
                    return true;
                }
            }
            else
            {
                clogE << "Wrong type: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(type) << "\n";
                return false;
            }
            break;
        default:
            clogE << "Unknown cluster ID: 0x" << std::hex << std::setw(4) << std::setfill('0') << clusterId << "\n";
            return false;
    }
}

bool CAppDemo::extractMultiClusterReport( std::vector<uint8_t > payload )
{
    size_t usedBytes = 0;
    bool validBuffer = true;

    while (payload.size()>0 && validBuffer)
    {
        validBuffer = extractClusterReport(payload, usedBytes);
        if (validBuffer)
        {
            payload.erase(payload.begin(), payload.begin()+usedBytes);
        }
    }
    return validBuffer;
}

void CAppDemo::handleRxGpFrame( CGpFrame &i_gpf )
{
    // Start DEBUG
    clogI << "CAppDemo::handleRxGpFrame gp frame : " << i_gpf <</*
        ", last hop rssi : " << unsigned(last_hop_rssi) <<
        ", from : "<< std::hex << std::setw(4) << std::setfill('0') << unsigned(sender) <<*/
        std::endl;

    // Stop DEBUG

    auto payloadSize = i_gpf.getPayload().size();

    clogD << "Received an attribute reporting frame (" << payloadSize << " bytes)";
    if (payloadSize!=0)
    {
        clogD << ": ";
        for (auto i : i_gpf.getPayload())
        {
            clogD << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i) << " ";
        }
    }
    clogD << "\n";

    switch(i_gpf.getCommandId())
    {
        case 0xa0:	/* Attribute reporting */
        {
            size_t usedBytes;
            if (!CAppDemo::extractClusterReport(i_gpf.getPayload(), usedBytes))
            {
                clogE << "Failed decoding attribute reporting payload: ";
                for (auto i : i_gpf.getPayload())
                {
                    clogE << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i) << " ";
                }
            }
        }
        break;

        case 0xa2:	/* Multi-Cluster Reporting */
        {
            if (!CAppDemo::extractMultiClusterReport(i_gpf.getPayload()))
            {
                clogE << "Failed to fully decode multi-cluster reporting payload: ";
                for (auto i : i_gpf.getPayload())
                {
                    clogE << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i) << " ";
                }
            }
        }
        break;

        default:
            clogW << "Unknown command ID: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_gpf.getCommandId()) << "\n";
            break;
    }
//    if( GPD_NO_SECURITY == i_gpf.getSecurity() )
//    {
//        gp_sink.registerGpd(i_gpf.getSourceId());
//    }
}

void CAppDemo::handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive ) {
    //-- clogI << "CAppDemo::ezspHandler " << CEzspEnum::EEzspCmdToString(i_cmd) << std::endl;

    switch( i_cmd )
    {
        case EZSP_STACK_STATUS_HANDLER:
        {
            EEmberStatus status = static_cast<EEmberStatus>(i_msg_receive.at(0));
            clogI << "CEZSP_STACK_STATUS_HANDLER status : " << CEzspEnum::EEmberStatusToString(status) << std::endl;
            if( (EMBER_NETWORK_UP == status) && (false == reset_wanted) )
            {
                setAppState(APP_READY);

                // we open network, so we can enter new devices
                zb_nwk.OpenNetwork( 60 );

                // we retrieve network information and key and eui64 of dongle (can be done before)
                dongle.sendCommand(EZSP_GET_NETWORK_PARAMETERS);
                dongle.sendCommand(EZSP_GET_EUI64);
                std::vector<uint8_t> l_payload;
                l_payload.push_back(EMBER_CURRENT_NETWORK_KEY);
                dongle.sendCommand(EZSP_GET_KEY, l_payload);

                // start discover of existing product inside network
                zb_nwk.startDiscoverProduct([&](EmberNodeType i_type, EmberEUI64 i_eui64, EmberNodeId i_id){
                    clogI << " Is it a new product ";
                    clogI << "[type : "<< CEzspEnum::EmberNodeTypeToString(i_type) << "]";
                    clogI << "[eui64 :";
                    for(uint8_t loop=0; loop<i_eui64.size(); loop++){ clogI << " " << std::hex << std::setw(2) << std::setfill('0') << unsigned(i_eui64[loop]); }
                    clogI << "]";
                    clogI << "[id : "<< std::hex << std::setw(4) << std::setfill('0') << unsigned(i_id) << "]";
                    clogI << " ?" << std::endl;

                    if( db.addProduct( i_eui64, i_id ) )
                    {
                        clogI << "YES !! Retrieve information for binding" << std::endl;

                        // retrieve information about device, starting by discover list of active endpoint
                        std::vector<uint8_t> payload;
                        payload.push_back(u16_get_lo_u8(i_id));
                        payload.push_back(u16_get_hi_u8(i_id));

                        zb_messaging.SendZDOCommand( i_id, ZDP_ACTIVE_EP, payload );
                    }
                });
            }
            else
            {
                clogD << "Call EZSP_NETWORK_STATE" << std::endl;
                dongle.sendCommand(EZSP_NETWORK_STATE);
            }
        }
        break;
        case EZSP_GET_NETWORK_PARAMETERS:
        {
            CGetNetworkParamtersResponse l_rsp(i_msg_receive);
            clogI << l_rsp.String() << std::endl;
        }
        break;
        case EZSP_GET_KEY:
        {
            EEmberStatus l_status = static_cast<EEmberStatus>(i_msg_receive.at(0));
            i_msg_receive.erase(i_msg_receive.begin());
            CEmberKeyStruct l_rsp(i_msg_receive);
            clogI << "EZSP_GET_KEY status : " << CEzspEnum::EEmberStatusToString(l_status) << ", " << l_rsp.String() << std::endl;
        }
        break;
        case EZSP_GET_EUI64:
        {
            // put eui64 on database for later use
            db.dongleEui64.clear();
            for( uint8_t loop=0; loop<EMBER_EUI64_BYTE_SIZE; loop++ )
            {
                db.dongleEui64.push_back(i_msg_receive.at(loop));
            }
        }
        break;
        case EZSP_VERSION:
        {
            // check if the wanted protocol version, and display stack version
            if( i_msg_receive.at(0) > ezsp_version )
            {
                ezsp_version = i_msg_receive.at(0);
                dongleInit();
            }
            if( i_msg_receive.at(0) == ezsp_version )
            {
                // all is good
                std::stringstream bufDump;

                // prtocol
                bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[0]) << ".";

                // type
                bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[1]) << ".";

                // version
                uint16_t l_version = dble_u8_to_u16(i_msg_receive[3], i_msg_receive[2]);
                bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(l_version);

                clogI << "Stack version : " << bufDump.str() << std::endl;

                // configure stack for this application
                stackInit();
            }
            else
            {
                clogI << "EZSP version " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_msg_receive[0]) << " Not supported !" << std::endl;
            }
        }
        break;
        case EZSP_NETWORK_STATE:
        {
            clogI << "CAppDemo::stackInit Return EZSP_NETWORK_STATE : " << unsigned(i_msg_receive.at(0)) << std::endl;
            if( EMBER_NO_NETWORK == i_msg_receive.at(0) )
            {
                // We create an HA1.2 network on the required channel
                if( APP_INIT_IN_PROGRESS == app_state )
                {
                    clogI << "CAppDemo::stackInit Call formHaNetwork" << std::endl;
                    zb_nwk.formHaNetwork(static_cast<uint8_t>(channel));
                    //set new state
                    setAppState(APP_FORM_NWK_IN_PROGRESS);
                    reset_wanted = false;
                }
                
            }
            else
            {
                if(( APP_INIT_IN_PROGRESS == app_state ) && ( true == reset_wanted ))
                {
                    // leave current network
                    zb_nwk.LeaveNetwork();
                    setAppState(APP_LEAVE_IN_PROGRESS);
                    reset_wanted = false;
                }
            }
        }
        break;
        case EZSP_LEAVE_NETWORK:
        {
            // set new state as initialized state
            setAppState(APP_INIT_IN_PROGRESS);
        }
        break;
        case EZSP_INCOMING_MESSAGE_HANDLER:
        {
            // the most important function where all zigbee incomming message arrive
            EmberIncomingMessageType type = static_cast<EmberIncomingMessageType>(i_msg_receive.at(0));
            std::vector<uint8_t> l_aps_raw;
            for(uint8_t loop=0;loop<CAPSFrame::getSize();loop++){l_aps_raw.push_back(i_msg_receive.at(1U+loop));}
            //uint8_t last_hop_lqi = i_msg_receive.at(1U+CAPSFrame::getSize());
            uint8_t last_hop_rssi = i_msg_receive.at(2U+CAPSFrame::getSize());
            EmberNodeId sender = static_cast<EmberNodeId>(dble_u8_to_u16(i_msg_receive.at(4U+CAPSFrame::getSize()), i_msg_receive.at(3U+CAPSFrame::getSize())));
            //uint8_t binding_idx = i_msg_receive.at(5U+CAPSFrame::getSize());
            //uint8_t address_idx = i_msg_receive.at(6U+CAPSFrame::getSize());
            uint8_t msg_length = i_msg_receive.at(7U+CAPSFrame::getSize());
            std::vector<uint8_t>l_msg_raw;
            for(uint8_t loop=0;loop<msg_length;loop++){l_msg_raw.push_back(i_msg_receive.at(8U+CAPSFrame::getSize()+loop));}

            clogI << "EZSP_INCOMING_MESSAGE_HANDLER type : " << CEzspEnum::EmberIncomingMessageTypeToString(type) << 
                ", last hop rssi : " << unsigned(last_hop_rssi) << 
                ", from : "<< std::hex << std::setw(4) << std::setfill('0') << unsigned(sender) << std::endl;

            // not process some type of messages
            if( !((EMBER_INCOMING_BROADCAST_LOOPBACK==type) || (EMBER_INCOMING_MULTICAST_LOOPBACK==type) || (EMBER_INCOMING_MANY_TO_ONE_ROUTE_REQUEST==type)) )
            {
                // build a zigbee message to simplify process
                CZigBeeMsg zbMsg;
                zbMsg.Set(l_aps_raw,l_msg_raw);

                if( 0 == zbMsg.GetAps().src_ep )
                {
                    EZdpLowByte zdp_low = static_cast<EZdpLowByte>(u16_get_lo_u8(zbMsg.GetAps().cluster_id));

                    // Zigbee Device Object
                    if( ZDP_HIGHT_BYTE_RESPONSE == u16_get_hi_u8(zbMsg.GetAps().cluster_id))
                    {
                        switch(zdp_low)
                        {
                            case ZDP_ACTIVE_EP:
                            {
                                // byte 0 is for sequence number, ignore it!
                                uint8_t status = zbMsg.GetPayload().at(1U);
                                EmberNodeId address = static_cast<EmberNodeId>(dble_u8_to_u16(zbMsg.GetPayload().at(3U), zbMsg.GetPayload().at(2U)));
                                uint8_t ep_count = zbMsg.GetPayload().at(4U);
                                std::vector<uint8_t> ep_list;
                                for(uint8_t loop=0; loop<ep_count;loop++){ep_list.push_back(zbMsg.GetPayload().at(5U+loop));}

                                // DEBUG
                                clogI << CZdpEnum::ToString(zdp_low) << " Response with status : " << 
                                    std::hex << std::setw(2) << std::setfill('0') << unsigned(status) << std::endl;

                                // for each active endpoint request simple descriptor
                                std::vector<uint8_t> payload;
                                for(uint8_t loop=0; loop<ep_list.size();loop++)
                                {
                                    payload.clear();
                                    payload.push_back(u16_get_lo_u8(address));
                                    payload.push_back(u16_get_hi_u8(address));
                                    payload.push_back(ep_list.at(loop));
                                    zb_messaging.SendZDOCommand(address,ZDP_SIMPLE_DESC,payload);
                                }
                                
                            }
                            break;
                            case ZDP_SIMPLE_DESC:
                            {
                                // for this simple exemple when we receive a simple descriptor we loop on cluster server and we bind each cluster who is interresting for us.
                                // byte 0 is for sequence number, ignore it!
                                uint8_t status = zbMsg.GetPayload().at(1U);

                                if( 0 == status )
                                {
                                    EmberNodeId address = static_cast<EmberNodeId>(dble_u8_to_u16(zbMsg.GetPayload().at(3U), zbMsg.GetPayload().at(2U)));
                                    uint8_t lentgh = zbMsg.GetPayload().at(4U);
                                    uint8_t endpoint = zbMsg.GetPayload().at(5U);
                                    uint16_t profile_id = dble_u8_to_u16(zbMsg.GetPayload().at(7U), zbMsg.GetPayload().at(6U));
                                    uint16_t device_id = dble_u8_to_u16(zbMsg.GetPayload().at(9U), zbMsg.GetPayload().at(8U));
                                    uint8_t version = zbMsg.GetPayload().at(10U);
                                    uint8_t in_count = zbMsg.GetPayload().at(11U);
                                    uint8_t out_count = zbMsg.GetPayload().at(12U+(2U*in_count));
                                    std::vector<uint16_t> in_list;
                                    for(uint8_t loop=0; loop<in_count; loop++)
                                    {
                                        in_list.push_back(dble_u8_to_u16(zbMsg.GetPayload().at(13U+(2U*loop)), zbMsg.GetPayload().at(12U+(2U*loop))));
                                    }
                                    std::vector<uint16_t> out_list;
                                    for(uint8_t loop=0; loop<out_count; loop++)
                                    {
                                        out_list.push_back(dble_u8_to_u16(zbMsg.GetPayload().at(14U+(2U*in_count)+(2U*loop)), zbMsg.GetPayload().at(13U+(2U*in_count)+(2U*loop))));
                                    }

                                    std::stringstream buf;
                                    buf << CZdpEnum::ToString(zdp_low) << " Response : " << 
                                        "[ status : " << std::hex << std::setw(2) << std::setfill('0') << unsigned(status) << "]" <<
                                        "[ address : " << std::hex << std::setw(4) << std::setfill('0') << unsigned(address) << "]" <<
                                        "[ lentgh : " << std::hex << std::setw(2) << std::setfill('0') << unsigned(lentgh) << "]" <<
                                        "[ endpoint : " << std::hex << std::setw(2) << std::setfill('0') << unsigned(endpoint) << "]" <<
                                        "[ profile_id : " << std::hex << std::setw(4) << std::setfill('0') << unsigned(profile_id) << "]" <<
                                        "[ device_id : " << std::hex << std::setw(4) << std::setfill('0') << unsigned(device_id) << "]" <<
                                        "[ version : " << std::hex << std::setw(2) << std::setfill('0') << unsigned(version) << "]" <<
                                        "[ in_count : " << std::hex << std::setw(2) << std::setfill('0') << unsigned(in_count) << "]" <<
                                        "[ in : ";
                                    for(uint8_t loop=0; loop<in_list.size(); loop++){ buf << std::hex << std::setw(4) << std::setfill('0') << unsigned(in_list.at(loop)) << ", "; }
                                    buf << "][ out_count : " << std::hex << std::setw(2) << std::setfill('0') << unsigned(out_count) << "]" <<
                                        "[ out : ";
                                    for(uint8_t loop=0; loop<out_list.size(); loop++){ buf << std::hex << std::setw(4) << std::setfill('0') << unsigned(out_list.at(loop)) << ", "; }
                                    buf << "]";
                                    clogI << buf.str() << std::endl;

                                    
                                    // search in server [in] list
                                    for(uint8_t loop=0; loop<in_list.size(); loop++)
                                    {
                                        // if we know one cluster we bind to it   
                                        if( 0x0402 == in_list.at(loop) )
                                        {
                                            // Temperature Measurement
                                            clogI << "<<< Bind Temperature" << std::endl;

                                            std::vector<uint8_t> payload;
                                            // source (product)
                                            // eui64
                                            EmberEUI64 src = db.getProductEui( address );
                                            payload.insert(payload.begin(),src.begin(),src.end());
                                            // ep
                                            payload.push_back(endpoint);
                                            // cluster
                                            payload.push_back(0x02);
                                            payload.push_back(0x04);

                                            // destination type (long address)
                                            payload.push_back(3);

                                            // destination (dongle)
                                            // eui64
                                            payload.insert(payload.end(),db.dongleEui64.begin(),db.dongleEui64.end());
                                            // ep
                                            payload.push_back(1);

                                            zb_messaging.SendZDOCommand(address,ZDP_BIND,payload);
                                        }
                                        else if( 0x0405 == in_list.at(loop) )
                                        {
                                            // Relative Humidity Measurement
                                            clogI << "<<< Bind Humidity" << std::endl;

                                            std::vector<uint8_t> payload;
                                            // source (product)
                                            // eui64
                                            EmberEUI64 src = db.getProductEui( address );
                                            payload.insert(payload.begin(),src.begin(),src.end());
                                            // ep
                                            payload.push_back(endpoint);
                                            // cluster
                                            payload.push_back(0x05);
                                            payload.push_back(0x04);

                                            // destination type (long address)
                                            payload.push_back(3);

                                            // destination (dongle)
                                            // eui64
                                            payload.insert(payload.end(),db.dongleEui64.begin(),db.dongleEui64.end());
                                            // ep
                                            payload.push_back(1);

                                            zb_messaging.SendZDOCommand(address,ZDP_BIND,payload);
                                        }
                                    }
                                }
                                else
                                {
                                    clogI << CZdpEnum::ToString(zdp_low) << " Response with status : " << 
                                        std::hex << std::setw(2) << std::setfill('0') << unsigned(status) << std::endl;
                                }
                            }
                            break;

                            default:
                            {
                                // DEBUG
                                clogI << "ZDO Response : " << CZdpEnum::ToString(zdp_low) << std::endl;
                            }
                            break;
                        }

                    }
                    else
                    {
                        if( ZDP_DEVICE_ANNOUNCE == zdp_low )
                        {
                            clogI << "ZDO Request : " << CZdpEnum::ToString(zdp_low) << std::endl;

                            // we receive a device announce because a child join or rejoin network, start a discover endpoint process

                            // byte 0 is for sequence number, ignore it!
                            EmberNodeId address = dble_u8_to_u16(zbMsg.GetPayload().at(2U), zbMsg.GetPayload().at(1U));
                            EmberEUI64 eui64;
                            for(uint8_t loop=0; loop<EMBER_EUI64_BYTE_SIZE; loop++){ eui64.push_back(zbMsg.GetPayload().at(3U+loop)); }

                            // add to database
                            db.addProduct( eui64, address );

                            std::vector<uint8_t> payload;
                            payload.push_back(u16_get_lo_u8(address));
                            payload.push_back(u16_get_hi_u8(address));

                            zb_messaging.SendZDOCommand( address, ZDP_ACTIVE_EP, payload );
                        }
                        else
                        {
                            // DEBUG
                            clogI << "ZDO Request : " << CZdpEnum::ToString(zdp_low) << std::endl;
                        }

                    }
                }
                else
                {
                    // applicative zigbee message
                    std::stringstream buf;
                    buf << "ZCL : " << 
                        "[ endpoint : " << std::hex << std::setw(2) << std::setfill('0') << unsigned(zbMsg.GetAps().src_ep) << "]" <<
                        "[ cluster : " << std::hex << std::setw(4) << std::setfill('0') << unsigned(zbMsg.GetAps().cluster_id) << "]" <<
                        "[ type : " << std::hex << std::setw(2) << std::setfill('0') << unsigned(zbMsg.GetZCLHeader().GetFrmCtrl().GetFrmType()) << "]" <<
                        "[ command : " << std::hex << std::setw(2) << std::setfill('0') << unsigned(zbMsg.GetZCLHeader().GetCmdId()) << "]" <<
                        "[ Data : ";
                    for(uint8_t loop=0; loop<zbMsg.GetPayload().size(); loop++)
                    {
                         buf << std::hex << std::setw(2) << std::setfill('0') << unsigned(zbMsg.GetPayload().at(loop)) << ", ";
                    }
                    buf << "]";
                    clogI << buf.str() << std::endl;

                    if( E_FRM_TYPE_GENERAL == zbMsg.GetZCLHeader().GetFrmCtrl().GetFrmType() )
                    {
                        // to be inprove : report attribute general command, be carrefull if they are two repport of same cluster in the frame
                        if( 0x0A == zbMsg.GetZCLHeader().GetCmdId() )
                        {
                            uint16_t attr_id = dble_u8_to_u16(zbMsg.GetPayload().at(1), zbMsg.GetPayload().at(0));
                            //uint8_t data_type = zbMsg.GetPayload().at(2);

                            if( 0x0402 == zbMsg.GetAps().cluster_id )
                            {
                                if( 0x00 == attr_id )
                                {
                                    uint16_t value_raw = dble_u8_to_u16(zbMsg.GetPayload().at(4), zbMsg.GetPayload().at(3));
                                    clogI << ">>> Temperature : " << static_cast<float>(value_raw) / 100 << "°C\n";
                                }
                            }
                            else if( 0x0405 == zbMsg.GetAps().cluster_id )
                            {
                                if( 0x00 == attr_id )
                                {
                                    uint16_t value_raw = dble_u8_to_u16(zbMsg.GetPayload().at(4), zbMsg.GetPayload().at(3));
                                    clogI << ">>> Relative Humidity : " << static_cast<float>(value_raw) / 100 << "%\n";
                                }
                            }
                        }
                    }
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
            clogI << "CAppDemo::ezspHandler : " << bufDump.str() << std::endl;
            */
        }
        break;
    }
}


/**
 * 
 * PRIVATE FUNCTIONS
 *  
 */

/**
 * utility function can managed error state
 */
void CAppDemo::setAppState( EAppState i_state )
{
    app_state = i_state;

    const std::map<EAppState,std::string> MyEnumStrings {
        { APP_NOT_INIT, "APP_NOT_INIT" },
        { APP_READY, "APP_READY" },
        { APP_ERROR, "APP_ERROR" },
        { APP_INIT_IN_PROGRESS, "APP_INIT_IN_PROGRESS" },
        { APP_FORM_NWK_IN_PROGRESS, "APP_FORM_NWK_IN_PROGRESS" },
    };

    auto  it  = MyEnumStrings.find(app_state); /* FIXME: we issue a warning, but the variable app_state is now out of bounds */
    std::string error_str = it == MyEnumStrings.end() ? "OUT_OF_RANGE" : it->second;
    clogI << "APP State change : " << error_str << std::endl;
}

void CAppDemo::dongleInit()
{
    // first request stack protocol version
    std::vector<uint8_t> payload;
    payload.push_back(ezsp_version);
    dongle.sendCommand(EZSP_VERSION,payload);
}

void CAppDemo::stackInit()
{
	std::vector<SEzspConfig> l_config;

	l_config.push_back({.id = EZSP_CONFIG_NEIGHBOR_TABLE_SIZE,
	                    .value = 32});
	l_config.push_back({.id = EZSP_CONFIG_APS_UNICAST_MESSAGE_COUNT,
	                    .value = 10});
	l_config.push_back({.id = EZSP_CONFIG_BINDING_TABLE_SIZE,
	                    .value = 0});
	l_config.push_back({.id = EZSP_CONFIG_ADDRESS_TABLE_SIZE,
	                    .value = 64});
	l_config.push_back({.id = EZSP_CONFIG_MULTICAST_TABLE_SIZE,
	                    .value = 8});
	l_config.push_back({.id = EZSP_CONFIG_ROUTE_TABLE_SIZE,
	                    .value = 32});
	l_config.push_back({.id = EZSP_CONFIG_DISCOVERY_TABLE_SIZE,
	                    .value = 16});
	l_config.push_back({.id = EZSP_CONFIG_STACK_PROFILE,
	                    .value = 2});
	l_config.push_back({.id = EZSP_CONFIG_SECURITY_LEVEL,
	                    .value = 5});
	l_config.push_back({.id = EZSP_CONFIG_MAX_HOPS,
	                    .value = 15});
	l_config.push_back({.id = EZSP_CONFIG_MAX_END_DEVICE_CHILDREN,
	                    .value = 32}); // define number of sleepy end device directly attached to dongle
	l_config.push_back({.id = EZSP_CONFIG_INDIRECT_TRANSMISSION_TIMEOUT,
	                    .value = 3000});
	l_config.push_back({.id = EZSP_CONFIG_END_DEVICE_POLL_TIMEOUT,
	                    .value = 5});
	l_config.push_back({.id = EZSP_CONFIG_MOBILE_NODE_POLL_TIMEOUT,
	                    .value = 20});
	l_config.push_back({.id = EZSP_CONFIG_RESERVED_MOBILE_CHILD_ENTRIES,
	                    .value = 0});
	l_config.push_back({.id = EZSP_CONFIG_TX_POWER_MODE,
	                    .value = 0});
	l_config.push_back({.id = EZSP_CONFIG_DISABLE_RELAY,
	                    .value = 0});
	l_config.push_back({.id = EZSP_CONFIG_TRUST_CENTER_ADDRESS_CACHE_SIZE,
	                    .value = 0});
	l_config.push_back({.id = EZSP_CONFIG_SOURCE_ROUTE_TABLE_SIZE,
	                    .value = 0});
	l_config.push_back({.id = EZSP_CONFIG_END_DEVICE_POLL_TIMEOUT_SHIFT,
	                    .value = 6});
	l_config.push_back({.id = EZSP_CONFIG_FRAGMENT_WINDOW_SIZE,
	                    .value = 0});
	l_config.push_back({.id = EZSP_CONFIG_FRAGMENT_DELAY_MS,
	                    .value = 0});
	l_config.push_back({.id = EZSP_CONFIG_KEY_TABLE_SIZE,
	                    .value = 12});
	l_config.push_back({.id = EZSP_CONFIG_APS_ACK_TIMEOUT,
	                    .value = (50*30)+100});
	l_config.push_back({.id = EZSP_CONFIG_BEACON_JITTER_DURATION,
	                    .value = 3});
	l_config.push_back({.id = EZSP_CONFIG_END_DEVICE_BIND_TIMEOUT,
	                    .value = 60});
	l_config.push_back({.id = EZSP_CONFIG_PAN_ID_CONFLICT_REPORT_THRESHOLD,
	                    .value = 1});
	l_config.push_back({.id = EZSP_CONFIG_REQUEST_KEY_TIMEOUT,
	                    .value = 0});
	/*l_config.push_back({.id = EZSP_CONFIG_CERTIFICATE_TABLE_SIZE,
	                    .value = 1});*/
	l_config.push_back({.id = EZSP_CONFIG_APPLICATION_ZDO_FLAGS,
	                    .value =0});
	l_config.push_back({.id = EZSP_CONFIG_BROADCAST_TABLE_SIZE,
	                    .value = 15});
	l_config.push_back({.id = EZSP_CONFIG_MAC_FILTER_TABLE_SIZE,
	                    .value = 0});
	l_config.push_back({.id = EZSP_CONFIG_SUPPORTED_NETWORKS,
	                    .value = 1});
	l_config.push_back({.id = EZSP_CONFIG_SEND_MULTICASTS_TO_SLEEPY_ADDRESS,
	                    .value = 0});
	l_config.push_back({.id = EZSP_CONFIG_ZLL_GROUP_ADDRESSES,
	                    .value = 0});
	/*l_config.push_back({.id = EZSP_CONFIG_ZLL_RSSI_THRESHOLD,
	                    .value = -128});*/
	l_config.push_back({.id = EZSP_CONFIG_MTORR_FLOW_CONTROL,
	                    .value = 1});
	l_config.push_back({.id = EZSP_CONFIG_RETRY_QUEUE_SIZE,
	                    .value = 8});
	l_config.push_back({.id = EZSP_CONFIG_NEW_BROADCAST_ENTRY_THRESHOLD,
	                    .value = 10});
	l_config.push_back({.id = EZSP_CONFIG_TRANSIENT_KEY_TIMEOUT_S,
	                    .value = 300});
	l_config.push_back({.id = EZSP_CONFIG_BROADCAST_MIN_ACKS_NEEDED,
	                    .value = 1});
	l_config.push_back({.id = EZSP_CONFIG_TC_REJOINS_USING_WELL_KNOWN_KEY_TIMEOUT_S,
	                    .value = 600});
	l_config.push_back({.id = EZSP_CONFIG_PACKET_BUFFER_COUNT,
	                    .value = 0xFF}); // use all remain memory for in/out radio packets

	std::vector<SEzspPolicy> l_policy;
	l_policy.push_back({.id = EZSP_TRUST_CENTER_POLICY,
	                    .decision = EZSP_ALLOW_PRECONFIGURED_KEY_JOINS});
	l_policy.push_back({.id = EZSP_MESSAGE_CONTENTS_IN_CALLBACK_POLICY,
	                    .decision = EZSP_MESSAGE_TAG_ONLY_IN_CALLBACK});
	l_policy.push_back({.id = EZSP_BINDING_MODIFICATION_POLICY,
	                    .decision = EZSP_CHECK_BINDING_MODIFICATIONS_ARE_VALID_ENDPOINT_CLUSTERS});
	l_policy.push_back({.id = EZSP_POLL_HANDLER_POLICY,
	                    .decision = EZSP_POLL_HANDLER_IGNORE});

	zb_nwk.stackInit(l_config, l_policy);
}

