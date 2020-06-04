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

/* Forward declaration of NSEZSP::EzspAdapterVersion */
namespace NSEZSP {
	class LIBEXPORT EzspAdapterVersion;
} // namespace NSEZSP

/* Declaration of friend operators belonging to :: */

/**
 * @brief Serialize an NSEZSP::EzspAdapterVersion instance to an iostream
 *
 * @param out The original output stream
 * @param data The object to serialize
 *
 * @return The new output stream with serialized data appended
 */
LIBEXPORT std::ostream& operator<<(std::ostream& out, const NSEZSP::EzspAdapterVersion& data);

/**
 * @brief Compare two NSEZSP::EzspAdapterVersion instances
 *
 * @param lhs The left-handle side instance compared
 * @param rhs The right-handle side instance compared
 *
 * @return true if instances are considered equivalent
 */
LIBEXPORT bool operator==(const NSEZSP::EzspAdapterVersion& lhs, const NSEZSP::EzspAdapterVersion& rhs);

/**
 * @brief Compare two EzspAdapterVersion instances
 *
 * @param lhs The left-handle side instance compared
 * @param rhs The right-handle side instance compared
 *
 * @return true if instances are considered different
 */
LIBEXPORT bool operator!=(const NSEZSP::EzspAdapterVersion& lhs, const NSEZSP::EzspAdapterVersion& rhs);

/**
 * @brief Compare two NSEZSP::EzspAdapterVersion instances
 *
 * @param lhs The left-handle side instance compared
 * @param rhs The right-handle side instance compared
 *
 * @return true if lhs is stricly greater than rhs
 */
LIBEXPORT bool operator>(const NSEZSP::EzspAdapterVersion& lhs, const NSEZSP::EzspAdapterVersion& rhs);

/**
 * @brief Compare two NSEZSP::EzspAdapterVersion instances
 *
 * @param lhs The left-handle side instance compared
 * @param rhs The right-handle side instance compared
 *
 * @return true if lhs is greater or equal to rhs
 */
LIBEXPORT bool operator>=(const NSEZSP::EzspAdapterVersion& lhs, const NSEZSP::EzspAdapterVersion& rhs);

/**
 * @brief Compare two NSEZSP::EzspAdapterVersion instances
 *
 * @param lhs The left-handle side instance compared
 * @param rhs The right-handle side instance compared
 *
 * @return true if lhs is stricly lower than rhs
 */
LIBEXPORT bool operator<(const NSEZSP::EzspAdapterVersion& lhs, const NSEZSP::EzspAdapterVersion& rhs);

/**
 * @brief Compare two NSEZSP::EzspAdapterVersion instances
 *
 * @param lhs The left-handle side instance compared
 * @param rhs The right-handle side instance compared
 *
 * @return true if lhs is lower or equal to rhs
 */
LIBEXPORT bool operator<=(const NSEZSP::EzspAdapterVersion& lhs, const NSEZSP::EzspAdapterVersion& rhs);

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

	EzspAdapterVersion();
	~EzspAdapterVersion() = default;

	/**
	 * @brief Store the EZSP stack version present inside an EZSP version info packet
	 *
	 * @param ezspStackVersion The EZSP stack version
	 */
	void setEzspVersionInfo(uint16_t ezspStackVersion);

	/**
	 * @brief Store EZSP version data present inside an EZSP version info packet
	 *
	 * @param ezspStackVersion The EZSP stack version
	 * @param ezspProtocolVersion The EZSP protocol version (EZSPv7, EZSPv8)
	 * @param ezspStackType The EZSP stack type
	 */
	void setEzspVersionInfo(uint16_t ezspStackVersion, uint8_t ezspProtocolVersion, uint8_t ezspStackType);

	/**
	 * @brief Store EZSP XNCP data present inside an EZSP XNCP info packet
	 *
	 * @param xncpManufacturerId The manufacturer ID (16-bit ID)
	 * @param xncpVersionNumber The Legrand XNCP 16-bit encoded hardware+firmware version (encoding is proprietary from Legrand)
	 */
	void setXncpData(uint16_t xncpManufacturerId, uint16_t xncpVersionNumber);

	/**
	 * @brief Get a string representation of the XNCP firmware version data
	 *
	 * @return The firmware version as a string
	 */
	std::string getFirmwareVersionAsString() const;

	/**
	 * @brief Get a string representation of the embedded stack version data
	 *
	 * @return The stack version as a string
	 */
	std::string getStackVersionAsString() const;

	/**
	 * @brief Represent the information stored by this instance as a string
	 * 
	 * @result The resulting string
	 */
	std::string toString() const;

	friend std::ostream& ::operator<<(std::ostream& out, const EzspAdapterVersion& data);

	friend bool ::operator==(const EzspAdapterVersion& lhs, const EzspAdapterVersion& rhs);

	friend bool ::operator!=(const EzspAdapterVersion& lhs, const EzspAdapterVersion& rhs) {
		return !(lhs == rhs);
	}

	friend bool ::operator>(const EzspAdapterVersion& lhs, const EzspAdapterVersion& rhs);

	friend bool ::operator>=(const EzspAdapterVersion& lhs, const EzspAdapterVersion& rhs) {
		return (lhs == rhs) || (lhs > rhs);
	}

	friend bool ::operator<(const EzspAdapterVersion& lhs, const EzspAdapterVersion& rhs);

	friend bool ::operator<=(const EzspAdapterVersion& lhs, const EzspAdapterVersion& rhs) {
		return (lhs == rhs) || (lhs < rhs);
	}

	unsigned int ezspProtocolVersion;       /*<! The EZSP protocol version EZSPv7, EZSPv8 */
	uint8_t ezspStackType;  /*<! The EZSP stack type (2 being mesh stack) */
	unsigned int ezspStackMajorVersion;   /*<! The EZSP adapter embedded stack major version */
	unsigned int ezspStackMinorVersion;   /*<! The EZSP adapter embedded stack minor version */
	unsigned int ezspStackRevisionVersion;   /*<! The EZSP adapter embedded stack revision version */
	unsigned int ezspStackBugfixVersion;   /*<! The EZSP adapter embedded stack bugfix version */
	uint16_t xncpManufacturerId;    /*<! The manufacturer ID (16-bit ID). 0x1021 is the ID registered for Legrand at the Zigbee Alliance */
	unsigned int xncpAdapterHardwareVersion;    /*<! The Legrand XNCP EZSP adapter hardware version (valid only if manufacturer is Legrand) */
	unsigned int xncpAdapterMajorVersion;   /*<! The Legrand XNCP EZSP adapter firmware major version (valid only if manufacturer is Legrand) */
	unsigned int xncpAdapterMinorVersion;   /*<! The Legrand XNCP EZSP adapter firmware minor version (valid only if manufacturer is Legrand) */
	unsigned int xncpAdapterRevisionVersion;   /*<! The Legrand XNCP EZSP adapter firmware revision version (valid only if manufacturer is Legrand) */
};

} // namespace NSEZSP

#endif // __EZSP_ADAPTER_VERSION_H__