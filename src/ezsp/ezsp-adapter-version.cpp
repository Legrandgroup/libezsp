/**
 * @file ezsp-adapter-version.cpp
 *
 * @brief Handling EZSP adapter versions (firmware and hardware)
 */

#include <sstream>
#include <iomanip>

#include <ezsp/ezsp-adapter-version.h>

#include "ezsp/byte-manip.h"

using NSEZSP::EzspAdapterVersion;

EzspAdapterVersion::EzspAdapterVersion() :
	ezspProtocolVersion(0),
	ezspStackType(0),
	ezspStackMajorVersion(0),
	ezspStackMinorVersion(0),
	ezspStackRevisionVersion(0),
	ezspStackBugfixVersion(0),
	xncpManufacturerId(static_cast<uint16_t>(Manufacturer::UNKNOWN)),
	xncpAdapterHardwareVersion(0),
	xncpAdapterMajorVersion(0),
	xncpAdapterMinorVersion(0),
	xncpAdapterRevisionVersion(0) {
}

void EzspAdapterVersion::setEzspVersionInfo(uint16_t ezspStackVersion) {
	this->ezspStackMajorVersion = u8_get_hi_nibble(u16_get_hi_u8(ezspStackVersion));
	this->ezspStackMinorVersion = u8_get_lo_nibble(u16_get_hi_u8(ezspStackVersion));
	this->ezspStackRevisionVersion = u8_get_hi_nibble(u16_get_lo_u8(ezspStackVersion));
	this->ezspStackBugfixVersion = u8_get_lo_nibble(u16_get_lo_u8(ezspStackVersion));
}

void EzspAdapterVersion::setEzspVersionInfo(uint16_t ezspStackVersion, uint8_t ezspProtocolVersion, uint8_t ezspStackType) {
	this->setEzspVersionInfo(ezspStackVersion);
	this->ezspProtocolVersion = ezspProtocolVersion;
	this->ezspStackType = ezspStackType;
}

void EzspAdapterVersion::setXncpData(uint16_t xncpManufacturerId, uint16_t xncpVersionNumber) {
	this->xncpManufacturerId = xncpManufacturerId;
	this->xncpAdapterHardwareVersion = u8_get_hi_nibble(u16_get_hi_u8(xncpVersionNumber)); /* High nibble of MSB */
	this->xncpAdapterMajorVersion = u8_get_lo_nibble(u16_get_hi_u8(xncpVersionNumber)); /* Low nibble of MSB */
	this->xncpAdapterMinorVersion = u8_get_hi_nibble(u16_get_lo_u8(xncpVersionNumber)); /* High nibble of LSB */
	this->xncpAdapterRevisionVersion = u8_get_lo_nibble(u16_get_lo_u8(xncpVersionNumber)); /* Low nibble of LSB */
}

void EzspAdapterVersion::setXncpData(EzspAdapterVersion::Manufacturer xncpManufacturerId, uint16_t xncpVersionNumber) {
	this->setXncpData(static_cast<uint16_t>(xncpManufacturerId), xncpVersionNumber);
}

std::string EzspAdapterVersion::getFirmwareVersionAsString() const {
	std::stringstream result;
	result << this->xncpAdapterMajorVersion << "." << this->xncpAdapterMinorVersion << "." << this->xncpAdapterRevisionVersion;
	return result.str();
}

std::string EzspAdapterVersion::getStackVersionAsString() const {
	std::stringstream result;
	result << static_cast<unsigned int>(this->ezspStackMajorVersion) << ".";
	result << static_cast<unsigned int>(this->ezspStackMinorVersion) << ".";
	result << static_cast<unsigned int>(this->ezspStackRevisionVersion) << ".";
	result << static_cast<unsigned int>(this->ezspStackBugfixVersion);
	return result.str();
}

std::string EzspAdapterVersion::toString() const {
	std::stringstream buf;

	buf << "EzspAdapterVersion : { ";
	if (this->xncpManufacturerId != static_cast<uint16_t>(Manufacturer::UNKNOWN)) {
		buf << "[Manufacturer: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(this->xncpManufacturerId);
		if (this->xncpManufacturerId == static_cast<uint16_t>(Manufacturer::LEGRAND)) {
			buf << " (LEGRAND)";
		}
		buf << "]";
	}
	if (this->ezspProtocolVersion) {
		buf << "[EZSPv" << std::dec << std::setw(0) << static_cast<unsigned int>(this->ezspProtocolVersion);
		buf << " running stack type " << static_cast<unsigned int>(this->ezspStackType);
		if (this->ezspStackType == 2) {
			buf << " (mesh)";
		}
		buf << "]";
	}
	if (this->ezspStackMajorVersion != 0
	    || this->ezspStackMinorVersion != 0
	    || this->ezspStackRevisionVersion != 0
	    || this->ezspStackBugfixVersion != 0) {
		buf << "[stack v" << this->getStackVersionAsString() << "]";
	}
	/* XNCP can only be properly decoded if manufacturer is Legrand */
	if (this->xncpManufacturerId == static_cast<uint16_t>(Manufacturer::LEGRAND)) {
		if (this->xncpAdapterHardwareVersion != 0
		    || this->xncpAdapterMajorVersion != 0
		    || this->xncpAdapterMinorVersion != 0
		    || this->xncpAdapterRevisionVersion != 0) {
			buf << "[hw v" << this->xncpAdapterHardwareVersion;
			buf << ", fw v" << this->getFirmwareVersionAsString() << "]";
		}
	}
	buf << " }";

	return buf.str();
}

/* External friend operators handling EzspAdapterVersion objects */
std::ostream& operator<<(std::ostream& out, const EzspAdapterVersion& data) {
	out << data.toString();
	return out;
}

int EzspAdapterVersion::cmp(const EzspAdapterVersion& lhs, const EzspAdapterVersion& rhs) {
	if (lhs.ezspProtocolVersion != 0 && rhs.ezspProtocolVersion != 0) {
		/* EZSP protocol version order matters first, but only if they are known (!=0) for both lhs and rhs */
		if (lhs.ezspProtocolVersion > rhs.ezspProtocolVersion) {
			return 1;
		}
		else if (lhs.ezspProtocolVersion < rhs.ezspProtocolVersion) {
			return -1;
		}
	}
	/* We reach here only when ezspProtocolVersion are assumed equivalent (or impossible to compare) */
	if (lhs.ezspStackType != 0 && rhs.ezspStackType != 0) {
		/* Stack type order then matters as a second step, but only if they are known (!=0) for both lhs and rhs */
		if (lhs.ezspStackType > rhs.ezspStackType) {
			//clogW << "Warning: comparison between two NSEZSP::EzspAdapterVersion instances holding a different stack type is dodgy\n";
			return 1;
		}
		else if (lhs.ezspStackType < rhs.ezspStackType) {
			//clogW << "Warning: comparison between two NSEZSP::EzspAdapterVersion instances holding a different stack type is dodgy\n";
			return -1;
		}
	}
	/* We reach here only when lhs.ezspStackType == rhs.ezspStackType */
	if (lhs.xncpManufacturerId == static_cast<uint16_t>(EzspAdapterVersion::Manufacturer::LEGRAND) && rhs.xncpManufacturerId == static_cast<uint16_t>(EzspAdapterVersion::Manufacturer::LEGRAND)) {
		/* Both adapters we compare are from Legrand... check only the XNCP versions (note: we don't compare the hardware revision, though) */
		if (lhs.xncpAdapterMajorVersion > rhs.xncpAdapterMajorVersion) {
			return 1;
		}
		else if (lhs.xncpAdapterMajorVersion < rhs.xncpAdapterMajorVersion) {
			return -1;
		}
		/* We reach here only when lhs.xncpAdapterMajorVersion == rhs.xncpAdapterMajorVersion */
		if (lhs.xncpAdapterMinorVersion > rhs.xncpAdapterMinorVersion) {
			return 1;
		}
		else if (lhs.xncpAdapterMinorVersion < rhs.xncpAdapterMinorVersion) {
			return -1;
		}
		/* We reach here only when lhs.xncpAdapterRevisionVersion == rhs.xncpAdapterRevisionVersion */
		if (lhs.xncpAdapterRevisionVersion > rhs.xncpAdapterRevisionVersion) {
			return 1;
		}
		else if (lhs.xncpAdapterRevisionVersion < rhs.xncpAdapterRevisionVersion) {
			return -1;
		}
	}
	else {	/* If at least one adapter is not from Legrand... */
		/* ...compare only the stack version */
		if (lhs.ezspStackMajorVersion > rhs.ezspStackMajorVersion) {
			return 1;
		}
		else if (lhs.ezspStackMajorVersion < rhs.ezspStackMajorVersion) {
			return -1;
		}
		if (lhs.ezspStackMinorVersion > rhs.ezspStackMinorVersion) {
			return 1;
		}
		else if (lhs.ezspStackMinorVersion < rhs.ezspStackMinorVersion) {
			return -1;
		}
		if (lhs.ezspStackRevisionVersion > rhs.ezspStackRevisionVersion) {
			return 1;
		}
		else if (lhs.ezspStackRevisionVersion < rhs.ezspStackRevisionVersion) {
			return -1;
		}
		if (lhs.ezspStackBugfixVersion > rhs.ezspStackBugfixVersion) {
			return 1;
		}
		else if (lhs.ezspStackBugfixVersion < rhs.ezspStackBugfixVersion) {
			return -1;
		}
	}
	return 0;	/* Identity */
}

bool operator==(const EzspAdapterVersion& lhs, const EzspAdapterVersion& rhs) {
	return ( EzspAdapterVersion::cmp(lhs, rhs) == 0);
}

bool operator!=(const EzspAdapterVersion& lhs, const EzspAdapterVersion& rhs) {
	return ( EzspAdapterVersion::cmp(lhs, rhs) != 0);
}

bool operator>(const EzspAdapterVersion& lhs, const EzspAdapterVersion& rhs) {
	return ( EzspAdapterVersion::cmp(lhs, rhs) > 0);
}

bool operator>=(const EzspAdapterVersion& lhs, const EzspAdapterVersion& rhs) {
	return ( EzspAdapterVersion::cmp(lhs, rhs) >= 0);
}

bool operator<(const EzspAdapterVersion& lhs, const EzspAdapterVersion& rhs) {
	return ( EzspAdapterVersion::cmp(lhs, rhs) < 0);
}

bool operator<=(const EzspAdapterVersion& lhs, const EzspAdapterVersion& rhs) {
	return ( EzspAdapterVersion::cmp(lhs, rhs) <= 0);
}
