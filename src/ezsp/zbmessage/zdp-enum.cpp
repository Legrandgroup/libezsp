/**
 *
 */
#include <map>

#include <ezsp/zbmessage/zdp-enum.h>

using NSEZSP::CZdpEnum;

std::string CZdpEnum::ToString( EZdpLowByte in ) {
	const std::map<EZdpLowByte,std::string> MyEnumStrings {
		{ ZDP_MGMT_PERMIT_JOINING_REQ, "ZDP_MGMT_PERMIT_JOINING_REQ" },
		{ ZDP_MGMT_BIND, "ZDP_MGMT_BIND" },
		{ ZDP_MGMT_LEAVE, "ZDP_MGMT_LEAVE" },
		{ ZDP_MGMT_RTG, "ZDP_MGMT_RTG" },
		{ ZDP_MGMT_LQI, "ZDP_MGMT_LQI" },
		{ ZDP_DISCOVERY_CACHE, "ZDP_DISCOVERY_CACHE" },
		{ ZDP_DEVICE_ANNOUNCE, "ZDP_DEVICE_ANNOUNCE" },
		{ ZDP_BIND, "ZDP_BIND" },
		{ ZDP_UNBIND, "ZDP_UNBIND" },
		{ ZDP_USER_DESC, "ZDP_USER_DESC" },
		{ ZDP_COMPLEX_DESC, "ZDP_COMPLEX_DESC" },
		{ ZDP_MATCH_DESC, "ZDP_MATCH_DESC" },
		{ ZDP_ACTIVE_EP, "ZDP_ACTIVE_EP" },
		{ ZDP_SIMPLE_DESC, "ZDP_SIMPLE_DESC" },
		{ ZDP_POWER_DESC, "ZDP_POWER_DESC" },
		{ ZDP_NODE_DESC, "ZDP_NODE_DESC" },
		{ ZDP_IEEE_ADDR, "ZDP_IEEE_ADDR" },
		{ ZDP_NWK_ADDR, "ZDP_NWK_ADDR" }
	};
	auto   it  = MyEnumStrings.find(in);
	return it == MyEnumStrings.end() ? "OUT_OF_RANGE" : it->second;
}
