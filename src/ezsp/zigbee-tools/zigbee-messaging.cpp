/**
 * @file zigbee-messaging.cpp
 *
 * @brief Manages zigbee message, timeout, retry
 */

#include "ezsp/zigbee-tools/zigbee-messaging.h"

#include "spi/ILogger.h"

#define GENERIC_COMMAND_READ_ATTRIBUTES                         0x00
#define GENERIC_COMMAND_WRITE_ATTRIBUTES                        0x02
#define GENERIC_COMMAND_CONFIGURE_ATTRIBUTES                    0x06
#define GENERIC_COMMAND_DISCOVER_ATTRIBUTES                     0x0C

using NSEZSP::CZigbeeMessaging;

CZigbeeMessaging::CZigbeeMessaging(CEzspDongle& i_dongle, const NSSPI::TimerBuilder& i_timer_builder) :
	dongle(i_dongle),
	timerBuilder(i_timer_builder) {
	dongle.registerObserver(this);
}

void CZigbeeMessaging::handleEzspRxMessage( EEzspCmd i_cmd, NSSPI::ByteBuffer i_msg_receive ) {
	switch( i_cmd ) {
	case EZSP_MESSAGE_SENT_HANDLER: {
		clogD << "EZSP_MESSAGE_SENT_HANDLER return status : " << CEzspEnum::EEmberStatusToString(static_cast<EEmberStatus>(i_msg_receive.at(16))) << std::endl;
	}
	break;
	default:
		break;
	}
}

void CZigbeeMessaging::SendBroadcast( EOutBroadcastDestination i_destination, uint8_t i_radius, CZigBeeMsg i_msg) {
	NSSPI::ByteBuffer l_payload;
	NSSPI::ByteBuffer l_zb_msg = i_msg.Get();

	// destination
	l_payload.push_back( static_cast<uint8_t>(i_destination&0xFF) );
	l_payload.push_back( static_cast<uint8_t>(static_cast<uint8_t>(i_destination>>8)&0xFF) );

	// aps frame
	NSSPI::ByteBuffer v_tmp = i_msg.GetAps().GetEmberAPS();
	l_payload.insert(l_payload.end(), v_tmp.begin(), v_tmp.end());

	// radius
	l_payload.push_back( i_radius );

	// message tag : not used for this simplier demo
	l_payload.push_back( 0 );

	// message length
	l_payload.push_back( static_cast<uint8_t>(l_zb_msg.size()) );

	// message content
	l_payload.insert(l_payload.end(), l_zb_msg.begin(), l_zb_msg.end());


	dongle.sendCommand(EZSP_SEND_BROADCAST, l_payload);
}

void CZigbeeMessaging::SendUnicast( EmberNodeId i_node_id, CZigBeeMsg i_msg ) {
	NSSPI::ByteBuffer l_payload;
	NSSPI::ByteBuffer l_zb_msg = i_msg.Get();

	// only direct unicast is supported for now
	l_payload.push_back( EMBER_OUTGOING_DIRECT );

	// destination
	l_payload.push_back( static_cast<uint8_t>(i_node_id&0xFF) );
	l_payload.push_back( static_cast<uint8_t>(static_cast<uint8_t>(i_node_id>>8)&0xFF) );

	// aps frame
	NSSPI::ByteBuffer v_tmp = i_msg.GetAps().GetEmberAPS();
	l_payload.insert(l_payload.end(), v_tmp.begin(), v_tmp.end());

	// message tag : not used for this simplier demo
	l_payload.push_back( 0 );

	// message length
	l_payload.push_back( static_cast<uint8_t>(l_zb_msg.size()) );

	// message content
	l_payload.insert(l_payload.end(), l_zb_msg.begin(), l_zb_msg.end());

	dongle.sendCommand(EZSP_SEND_UNICAST, l_payload);
}

void CZigbeeMessaging::SendZDOCommand(EmberNodeId i_node_id, uint16_t i_cmd_id, const NSSPI::ByteBuffer& payload) {
	CZigBeeMsg l_msg;
	l_msg.SetZdo( i_cmd_id, payload, 0/*network.GetNextTransactionNb(i_node_id, 0)*/ );
	SendUnicast( i_node_id, l_msg );
}

void CZigbeeMessaging::SendZCLCommand(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint8_t i_cmd_id,
									  const EZCLFrameCtrlDirection i_direction, const NSSPI::ByteBuffer& i_payload,
									  const uint16_t i_node_id, const uint8_t i_transaction_number,
									  const uint16_t i_grp_id, const uint16_t i_manufacturer_code) {
	CZigBeeMsg l_msg;
	uint16_t l_profile;
	if( 242 == i_endpoint ) {
		l_profile = 0xA1E0; // Green Power
	}
	else {
		l_profile = 0x0104; // 0xFFFFU;
	}
	l_msg.SetSpecific( l_profile, i_manufacturer_code, i_endpoint, i_cluster_id, i_cmd_id, i_direction, i_payload, i_transaction_number, i_grp_id);
	SendUnicast( i_node_id, l_msg );
}

void CZigbeeMessaging::DiscoverAttributes(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint16_t i_start_attribute_identifier,
										 const uint8_t i_maximum_attribute_identifier, const EZCLFrameCtrlDirection i_direction, const uint16_t i_node_id,
										 const uint8_t i_transaction_number, const uint16_t i_grp_id, const uint16_t i_manufacturer_code) {
	CZigBeeMsg l_msg;
	uint16_t l_profile;
	if( 242 == i_endpoint )
	{
		l_profile = 0xA1E0; // Green Power
	}
	else
	{
		l_profile = 0x0104; // 0xFFFFU;
	}

	NSSPI::ByteBuffer l_payload;
	l_payload.push_back( i_start_attribute_identifier&0xFF );
	l_payload.push_back( (i_start_attribute_identifier>>8)&0xFF );
	l_payload.push_back( i_maximum_attribute_identifier );

	l_msg.SetGeneral( l_profile, i_manufacturer_code, i_endpoint, i_cluster_id, GENERIC_COMMAND_DISCOVER_ATTRIBUTES, i_direction, l_payload, i_transaction_number, i_grp_id);
	SendUnicast( i_node_id, l_msg );
}

void CZigbeeMessaging::ReadAttributes(const uint8_t i_endpoint, const uint16_t i_cluster_id, const std::vector<uint16_t> &i_attribute_ids,
				   					 const EZCLFrameCtrlDirection i_direction, const uint16_t i_node_id,
				   					 const uint8_t i_transaction_number, const uint16_t i_grp_id, const uint16_t i_manufacturer_code) {
	CZigBeeMsg l_msg;
	uint16_t l_profile;
	if( 242 == i_endpoint )
	{
		l_profile = 0xA1E0; // Green Power
	}
	else
	{
		l_profile = 0x0104; // 0xFFFFU;
	}

	NSSPI::ByteBuffer l_payload;
	for (auto attribute : i_attribute_ids) {
		l_payload.push_back( attribute&0xFF );
		l_payload.push_back( (attribute>>8)&0xFF );
	}

	l_msg.SetGeneral( l_profile, i_manufacturer_code, i_endpoint, i_cluster_id, GENERIC_COMMAND_READ_ATTRIBUTES, i_direction, l_payload, i_transaction_number, i_grp_id);
	SendUnicast( i_node_id, l_msg );	
}

void CZigbeeMessaging::WriteAttribute(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint16_t i_attribute_id,
									  const EZCLFrameCtrlDirection i_direction, const uint8_t i_datatype, const NSSPI::ByteBuffer& i_data,
									  const uint16_t i_node_id, const uint8_t i_transaction_number,
									  const uint16_t i_grp_id, const uint16_t i_manufacturer_code) {
	CZigBeeMsg l_msg;
	uint16_t l_profile;
	if( 242 == i_endpoint )
	{
		l_profile = 0xA1E0; // Green Power
	}
	else
	{
		l_profile = 0x0104; // 0xFFFFU;
	}

	NSSPI::ByteBuffer l_payload;
	l_payload.push_back( i_attribute_id&0xFF );
	l_payload.push_back( (i_attribute_id>>8)&0xFF );
	l_payload.push_back( i_datatype );
	l_payload.append( i_data );

	l_msg.SetGeneral( l_profile, i_manufacturer_code, i_endpoint, i_cluster_id, GENERIC_COMMAND_WRITE_ATTRIBUTES, i_direction, l_payload, i_transaction_number, i_grp_id);
	SendUnicast( i_node_id, l_msg );
}

void CZigbeeMessaging::ConfigureReporting(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint16_t i_attribute_id,
										const EZCLFrameCtrlDirection i_direction, const uint8_t i_datatype, const uint16_t i_min,
										const uint16_t i_max, const uint16_t i_reportable, const uint16_t i_node_id,
										const uint8_t i_transaction_number, const uint16_t i_grp_id, const uint16_t i_manufacturer_code) {
	CZigBeeMsg l_msg;
	uint16_t l_profile;
	if( 242 == i_endpoint )
	{
		l_profile = 0xA1E0; // Green Power
	}
	else
	{
		l_profile = 0x0104; // 0xFFFFU;
	}

	NSSPI::ByteBuffer l_payload;
	l_payload.push_back( i_direction );
	l_payload.push_back( i_attribute_id&0xFF );
	l_payload.push_back( (i_attribute_id>>8)&0xFF );
	l_payload.push_back( i_datatype );
	l_payload.push_back( i_min&0xFF );
	l_payload.push_back( (i_min>>8)&0xFF );
	l_payload.push_back( i_max&0xFF );
	l_payload.push_back( (i_max>>8)&0xFF );
	l_payload.push_back( i_reportable&0xFF );
	l_payload.push_back( (i_reportable>>8)&0xFF );

	l_msg.SetGeneral( l_profile, i_manufacturer_code, i_endpoint, i_cluster_id, GENERIC_COMMAND_CONFIGURE_ATTRIBUTES, i_direction, l_payload, i_transaction_number, i_grp_id);
	SendUnicast( i_node_id, l_msg );
}
