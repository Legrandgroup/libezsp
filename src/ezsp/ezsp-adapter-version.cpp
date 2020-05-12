/**
 * @file ezsp-adapter-version.cpp
 *
 * @brief Handling EZSP adapter versions (firmware and hardware)
 */

#include <sstream>

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