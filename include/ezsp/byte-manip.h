/**
 * @brief Utility function to create unsigned 16-bit integers from their 8-bit high and low bytes and vice versa
 */

#pragma once

#include <cstdint>

namespace NSEZSP {

inline uint16_t dble_u8_to_u16(const uint8_t highByte, const uint8_t lowByte) {
	uint16_t value = static_cast<uint16_t>(highByte) << 8;
	value |= static_cast<uint16_t>(lowByte);
	return value;
}

inline uint32_t quad_u8_to_u32(const uint8_t highestByte3, const uint8_t byte2, const uint8_t byte1, const uint8_t lowestByte0) {
	uint32_t value = static_cast<uint32_t>(highestByte3) << 24;
	value |= static_cast<uint32_t>(byte2) << 16;
	value |= static_cast<uint32_t>(byte1) << 8;
	value |= static_cast<uint32_t>(lowestByte0);
	return value;
}

inline uint8_t u16_get_hi_u8(const uint16_t word) {
	return static_cast<uint8_t>(word >> 8);
}

inline uint8_t u16_get_lo_u8(const uint16_t word) {
	return static_cast<uint8_t>(word & 0xFF);
}

/**
 * @brief Get bits 7 to 0 of a 32-bit value
 *
 * @return The result as a byte (uint8_t)
**/
inline uint8_t u32_get_byte0(const uint32_t word32) {
	return static_cast<uint8_t>(word32 & 0xFF);
}

/**
 * @brief Get bits 15 to 8 of a 32-bit value
 *
 * @return The result as a byte (uint8_t)
**/
inline uint8_t u32_get_byte1(const uint16_t word32) {
	return static_cast<uint8_t>((word32>>8) & 0xFF);
}

/**
 * @brief Get bits 23 to 16 of a 32-bit value
 *
 * @return The result as a byte (uint8_t)
**/
inline uint8_t u32_get_byte2(const uint16_t word32) {
	return static_cast<uint8_t>((word32>>16) & 0xFF);
}

/**
 * @brief Get bits 31 to 24 of a 32-bit value
 *
 * @return The result as a byte (uint8_t)
**/
inline uint8_t u32_get_byte3(const uint16_t word32) {
	return static_cast<uint8_t>((word32>>24) & 0xFF);
}

} // namespace NSEZSP
