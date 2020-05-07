/**
 * @file ezsp-adapter-version.h
 *
 * @brief Handling EZSP adapter versions (firmware and hardware)
 */

#ifndef __EZSP_ADAPTER_VERSION_H__
#define __EZSP_ADAPTER_VERSION_H__

#include <string>
#include <cstdint>

#include <ezsp/export.h>

namespace NSEZSP {

/**
 * @brief Class storing information about the EZSP adapter
 */
class LIBEXPORT EzspAdapterVersion {
public:
	enum class Manufacturer {
		UNKNOWN = 0,
		LEGRAND = 0x1021
	};
	EzspAdapterVersion(uint16_t xncpManufacturerId, uint16_t xncpVersionNumber);
	EzspAdapterVersion();
	~EzspAdapterVersion() = default;

	std::string getFirmwareVersionAsString() const;

	uint16_t xncpManufacturerId;    /*<! The manufacturer ID (16-bit ID). 0x1021 is the ID registered for Legrand at the Zigbee Alliance */
	unsigned int xncpAdapterHardwareVersion;    /*<! The Legrand XNCP EZSP adapter hardware version (valid only if manufacturer is Legrand) */
	unsigned int xncpAdapterMajorVersion;   /*<! The Legrand XNCP EZSP adapter firmware major version (valid only if manufacturer is Legrand) */
	unsigned int xncpAdapterMinorVersion;   /*<! The Legrand XNCP EZSP adapter firmware minor version (valid only if manufacturer is Legrand) */
	unsigned int xncpAdapterRevisionVersion;   /*<! The Legrand XNCP EZSP adapter firmware revision version (valid only if manufacturer is Legrand) */
};

} // namespace NSEZSP

#endif // __EZSP_ADAPTER_VERSION_H__