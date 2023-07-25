/**
 * Command / payload definition for Zigbee Device object (endpoint 0)
 */
#pragma once

#include <string>
#include <cstdint>

namespace NSEZSP {

constexpr uint8_t ZDP_HIGHT_BYTE_REQUEST  = 0x00;
constexpr uint8_t ZDP_HIGHT_BYTE_RESPONSE = 0x80;

//ZDP LOW BYTE
typedef enum {
	ZDP_NWK_ADDR =         		  0x00,
	ZDP_IEEE_ADDR =         	  0x01,
	ZDP_NODE_DESC =         	  0x02,
	ZDP_POWER_DESC =        	  0x03,
	ZDP_SIMPLE_DESC =       	  0x04,
	ZDP_ACTIVE_EP =         	  0x05,
	ZDP_MATCH_DESC =        	  0x06,
	ZDP_COMPLEX_DESC =      	  0x10,
	ZDP_USER_DESC =         	  0x11,
	ZDP_DISCOVERY_CACHE =   	  0x12,
	ZDP_DEVICE_ANNOUNCE =   	  0x13,
	ZDP_BIND =   	  			  0x21,
	ZDP_UNBIND = 				  0x22,

	ZDP_MGMT_LQI =          	  0x31,
	ZDP_MGMT_RTG =  	    	  0x32,
	ZDP_MGMT_BIND =         	  0x33,
	ZDP_MGMT_LEAVE =         	  0x34,
	ZDP_MGMT_PERMIT_JOINING_REQ = 0x36
} EZdpLowByte;

class CZdpEnum {
public:
	static std::string ToString( EZdpLowByte in );
};

} // namespace NSEZSP
