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
	if (this->ezspStackMajorVersion || this->ezspStackMinorVersion || this->ezspStackRevisionVersion || this->ezspStackBugfixVersion) {
		buf << "[stack v" << this->getStackVersionAsString() << "]";
	}
	/* XNCP can only be properly decoded if manufacturer is Legrand */
	if (this->xncpManufacturerId == static_cast<uint16_t>(Manufacturer::LEGRAND)) {
		if (this->xncpAdapterHardwareVersion || this->xncpAdapterMajorVersion || this->xncpAdapterMinorVersion || this->xncpAdapterRevisionVersion) {
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

bool operator==(const EzspAdapterVersion& lhs, const EzspAdapterVersion& rhs) {
	/* In all cases, EZSP protocol version and stack types must match */
	if ( (lhs.ezspProtocolVersion != rhs.ezspProtocolVersion)
	     || (lhs.ezspStackType != rhs.ezspStackType) ) {
		return false;
	}
	if (lhs.xncpManufacturerId == static_cast<uint16_t>(EzspAdapterVersion::Manufacturer::LEGRAND) && rhs.xncpManufacturerId == static_cast<uint16_t>(EzspAdapterVersion::Manufacturer::LEGRAND)) {
		/* Both adapters we compare are from Legrand... check only the XNCP versions (note: we don't compare the hardware revision, though) */
		if ( (lhs.xncpAdapterMajorVersion != rhs.xncpAdapterMajorVersion)
		     || (lhs.xncpAdapterMinorVersion != rhs.xncpAdapterMinorVersion)
		     || (lhs.xncpAdapterRevisionVersion != rhs.xncpAdapterRevisionVersion) ) {
			return false;
		}
	}
	else {	/* If at least one adapter is not from Legrand... */
		/* ...compare only the stack version */
		if ( (lhs.ezspStackMajorVersion != rhs.ezspStackMajorVersion)
		     || (lhs.ezspStackMinorVersion != rhs.ezspStackMinorVersion)
		     || (lhs.ezspStackRevisionVersion != rhs.ezspStackRevisionVersion)
		     || (lhs.ezspStackBugfixVersion != rhs.ezspStackBugfixVersion) ) {
			return false;
		}
	}
	return true;
}

bool operator>(const EzspAdapterVersion& lhs, const EzspAdapterVersion& rhs) {
	/* EZSP protocol version order matters first */
	if (lhs.ezspProtocolVersion > rhs.ezspProtocolVersion) {
		return true;
	}
	if (lhs.ezspStackType > rhs.ezspStackType) {
		//clogW << "Warning: comparison between two NSEZSP::EzspAdapterVersion instances holding a different stack type is dodgy\n";
		return true;
	}
	if (lhs.xncpManufacturerId == static_cast<uint16_t>(EzspAdapterVersion::Manufacturer::LEGRAND) && rhs.xncpManufacturerId == static_cast<uint16_t>(EzspAdapterVersion::Manufacturer::LEGRAND)) {
		/* Both adapters we compare are from Legrand... check only the XNCP versions (note: we don't compare the hardware revision, though) */
		if (lhs.xncpAdapterMajorVersion > rhs.xncpAdapterMajorVersion) {
			return true;
		}
		if (lhs.xncpAdapterMinorVersion > rhs.xncpAdapterMinorVersion) {
			return true;
		}
		if (lhs.xncpAdapterRevisionVersion > rhs.xncpAdapterRevisionVersion) {
			return true;
		}
	}
	else {	/* If at least one adapter is not from Legrand... */
		/* ...compare only the stack version */
		if (lhs.ezspStackMajorVersion > rhs.ezspStackMajorVersion) {
			return true;
		}
		if (lhs.ezspStackMinorVersion > rhs.ezspStackMinorVersion) {
			return true;
		}
		if (lhs.ezspStackRevisionVersion > rhs.ezspStackRevisionVersion) {
			return true;
		}
		if (lhs.ezspStackBugfixVersion > rhs.ezspStackBugfixVersion) {
			return true;
		}
	}
	return false;
}

bool operator<(const EzspAdapterVersion& lhs, const EzspAdapterVersion& rhs) {
	/* EZSP protocol version order matters first */
	if (lhs.ezspProtocolVersion < rhs.ezspProtocolVersion) {
		return true;
	}
	if (lhs.ezspStackType < rhs.ezspStackType) {
		//clogW << "Warning: comparison between two NSEZSP::EzspAdapterVersion instances holding a different stack type is dodgy\n";
		return true;
	}
	if (lhs.xncpManufacturerId == static_cast<uint16_t>(EzspAdapterVersion::Manufacturer::LEGRAND) && rhs.xncpManufacturerId == static_cast<uint16_t>(EzspAdapterVersion::Manufacturer::LEGRAND)) {
		/* Both adapters we compare are from Legrand... check only the XNCP versions (note: we don't compare the hardware revision, though) */
		if (lhs.xncpAdapterMajorVersion < rhs.xncpAdapterMajorVersion) {
			return true;
		}
		if (lhs.xncpAdapterMinorVersion < rhs.xncpAdapterMinorVersion) {
			return true;
		}
		if (lhs.xncpAdapterRevisionVersion < rhs.xncpAdapterRevisionVersion) {
			return true;
		}
	}
	else {	/* If at least one adapter is not from Legrand... */
		/* ...compare only the stack version */
		if (lhs.ezspStackMajorVersion < rhs.ezspStackMajorVersion) {
			return true;
		}
		if (lhs.ezspStackMinorVersion < rhs.ezspStackMinorVersion) {
			return true;
		}
		if (lhs.ezspStackRevisionVersion < rhs.ezspStackRevisionVersion) {
			return true;
		}
		if (lhs.ezspStackBugfixVersion < rhs.ezspStackBugfixVersion) {
			return true;
		}
	}
	return false;
}
