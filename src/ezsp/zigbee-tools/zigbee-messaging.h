/**
 * @file zigbee-messaging.h
 *
 * @brief Manages zigbee message, timeout, retry
 */
#pragma once

#include "ezsp/ezsp-dongle-observer.h"
#include "ezsp/ezsp-dongle.h"
#include "ezsp/zbmessage/zigbee-message.h"
#include "spi/ByteBuffer.h"

#include <map>
#include <vector>

namespace NSEZSP {

class CZigbeeMessaging : public CEzspDongleObserver {
public:
	/**
	 * @brief Constructor
	 *
	 * @param[in] i_dongle The EZSP adapter used to send/receive Zigbee messages
	 * @param[in] i_timer_builder Timer builder object used to generate timers
	 */
	CZigbeeMessaging(CEzspDongle& i_dongle, const NSSPI::TimerBuilder& i_timer_builder);

	void SendBroadcast( EOutBroadcastDestination i_destination, uint8_t i_radius, CZigBeeMsg i_msg);
	void SendUnicast( EmberNodeId i_node_id, CZigBeeMsg i_msg );

	/**
	 * @brief Send a ZDO unicast command
	 *
	 * @param i_node_id Short address of destination
	 * @param i_cmd_id Command
	 * @param[in] payload Payload for the ZDO unicast
	 */
	void SendZDOCommand( EmberNodeId i_node_id, uint16_t i_cmd_id, const NSSPI::ByteBuffer& payload );

	/**
	 * @brief Send a ZCL unicast command
	 *
	 * @param i_node_id Short address of destination
	 * @param i_endpoint Destination endpoint
	 * @param i_cluster_id Concerned cluster
	 * @param i_cmd_id Command ID
	 * @param i_direction Message direction (client to server or server to client)
	 * @param i_payload Payload of the command
	 * @param i_grp_id Multicast group address to use (0 is assume as unicast/broadcast)
	 * @param i_manufacturer_code Manufacturer code
	 */
	void SendZCLCommand( const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint8_t i_cmd_id,
						 const EZCLFrameCtrlDirection i_direction, const NSSPI::ByteBuffer& i_payload,
						 const uint16_t i_node_id, const uint8_t i_transaction_number = 0, const uint16_t i_grp_id = 0, const uint16_t i_manufacturer_code = 0xFFFF);

	/**
	 * @brief Send a Discover Attributes command
	 *
	 * @param i_endpoint Destination endpoint
	 * @param i_cluster_id Concerned cluster
	 * @param i_start_attribute_identifier Specifies the value of the identifier at which to begin the attribute discovery.
	 * @param i_maximum_attribute_identifier specifies the maximum number of attribute identifiers that are to be returned in the resulting Discover Attributes Response command.
	 * @param i_direction Message direction (client to server or server to client)
	 * @param i_node_id Short address of destination
	 * @param i_grp_id Multicast group address to use (0 is assume as unicast/broadcast)
	 * @param i_manufacturer_code Manufacturer code
	 */
	void DiscoverAttributes(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint16_t i_start_attribute_identifier,
						    const uint8_t i_maximum_attribute_identifier, const EZCLFrameCtrlDirection i_direction, const uint16_t i_node_id,
						    const uint8_t i_transaction_number = 0, const uint16_t i_grp_id = 0, const uint16_t i_manufacturer_code = 0xFFFF);

	/**
	 * @brief Send a Read Attribute command
	 *
	 * @param i_endpoint Destination endpoint
	 * @param i_cluster_id Concerned cluster
	 * @param i_attribute_id Attribute id to read
	 * @param i_direction Message direction (client to server or server to client)
	 * @param i_node_id Short address of destination
	 * @param i_grp_id Multicast group address to use (0 is assume as unicast/broadcast)
	 * @param i_manufacturer_code Manufacturer code
	 */
	void ReadAttributes(const uint8_t i_endpoint, const uint16_t i_cluster_id, const std::vector<uint16_t> &i_attribute_id,
							const EZCLFrameCtrlDirection i_direction, const uint16_t i_node_id,
							const uint8_t i_transaction_number = 0, const uint16_t i_grp_id = 0, const uint16_t i_manufacturer_code = 0xFFFF);
	
	/**
	 * @brief Send a WriteAttribute command
	 *
	 * @param i_endpoint Destination endpoint
	 * @param i_cluster_id Concerned cluster
	 * @param i_attribute_id Attribute to write
	 * @param i_direction Message direction (client to server or server to client)
	 * @param i_datatype Type of data to be written
	 * @param i_data Data to write
	 * @param i_node_id Short address of destination
	 * @param i_grp_id Multicast group address to use (0 is assume as unicast/broadcast)
	 * @param i_manufacturer_code Manufacturer code
	 */
	void WriteAttribute(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint16_t i_attribute_id,
						const EZCLFrameCtrlDirection i_direction, const uint8_t i_datatype, const NSSPI::ByteBuffer& i_data,
						const uint16_t i_node_id, const uint8_t i_transaction_number = 0,
						const uint16_t i_grp_id = 0, const uint16_t i_manufacturer_code = 0xFFFF);

	/**
	 * @brief Send a Configure Reporting command
	 *
	 * @param i_endpoint Destination endpoint
	 * @param i_cluster_id Concerned cluster
	 * @param i_attribute_id Attribute id
	 * @param i_direction Message direction (client to server or server to client)
	 * @param i_datatype Attribute type
	 * @param i_min Minimum reporting interval
	 * @param i_max Maximum reporting interval
	 * @param i_reportable Reportable change
	 * @param i_node_id Short address of destination
	 * @param i_grp_id Multicast group address to use (0 is assume as unicast/broadcast)
	 * @param i_manufacturer_code Manufacturer code
	 */
	void ConfigureReporting(const uint8_t i_endpoint, const uint16_t i_cluster_id, const uint16_t i_attribute_id,
							const EZCLFrameCtrlDirection i_direction, const uint8_t i_datatype, const uint16_t i_min,
							const uint16_t i_max, const uint16_t i_reportable, const uint16_t i_node_id,
							const uint8_t i_transaction_number = 0, const uint16_t i_grp_id = 0, const uint16_t i_manufacturer_code = 0xFFFF);

	/**
	 * Observer
	 */
	void handleEzspRxMessage( EEzspCmd i_cmd, NSSPI::ByteBuffer i_msg_receive );

private:
	CEzspDongle &dongle;
	const NSSPI::TimerBuilder& timerBuilder; // needed in the future to well manage retry/timeout on unicast zigbee message
};

} // namespace NSEZSP
