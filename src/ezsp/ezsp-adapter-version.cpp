/**
 * @file ezsp-adapter-version.cpp
 *
 * @brief Handling EZSP adapter versions (firmware and hardware)
 */

#include <sstream>

#include <ezsp/ezsp-adapter-version.h>

#include "ezsp/byte-manip.h"

using NSEZSP::EzspAdapterVersion;

EzspAdapterVersion::EzspAdapterVersion(uint16_t xncpManufacturerId, uint16_t xncpVersionNumber) :
	xncpManufacturerId(xncpManufacturerId),
	xncpAdapterHardwareVersion(u16_get_hi_u8(xncpVersionNumber) >> 4), /* High nibble of MSB */
	xncpAdapterMajorVersion(u16_get_hi_u8(xncpVersionNumber) & 0x0f), /* Low nibble of MSB */
	xncpAdapterMinorVersion(u16_get_lo_u8(xncpVersionNumber) >> 4), /* High nibble of LSB */
	xncpAdapterRevisionVersion(u16_get_lo_u8(xncpVersionNumber) & 0x0f) { /* Low nibble of LSB */
}

EzspAdapterVersion::EzspAdapterVersion() :
	EzspAdapterVersion(static_cast<uint16_t>(Manufacturer::UNKNOWN), 0) {
}

std::string EzspAdapterVersion::getFirmwareVersionAsString() const {
	std::stringstream result;
	result << this->xncpAdapterMajorVersion << "." << this->xncpAdapterMinorVersion << "." << this->xncpAdapterRevisionVersion;
	return result.str();
}