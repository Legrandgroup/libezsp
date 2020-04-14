/**
 * @file byte-manip.h
 *
 * @brief Utility function to create unsigned 16-bit integers from their 8-bit high and low bytes and vice versa
 */

#pragma once

#include <cstdint>

namespace NSEZSP {

/**
 * @brief Create a 16-bit value from 2 bytes
 *
 * @param highByte A byte containing bits 15 to 8
 * @param lowByte A byte containing bits 7 to 0
 */
inline uint16_t dble_u8_to_u16(const uint8_t highByte, const uint8_t lowByte) {
	uint16_t value = static_cast<uint16_t>(highByte) << 8;
	value |= static_cast<uint16_t>(lowByte);
	return value;
}

/**
 * @brief Create a 32-bit value from 4 bytes
 *
 * @param highestByte3 A byte containing bits 31 to 24
 * @param byte2 A byte containing bits 23 to 16
 * @param byte1 A byte containing bits 15 to 8
 * @param lowestByte0 A byte containing bits 7 to 0
 */
inline uint32_t quad_u8_to_u32(const uint8_t highestByte3, const uint8_t byte2, const uint8_t byte1, const uint8_t lowestByte0) {
	uint32_t value = static_cast<uint32_t>(highestByte3) << 24;
	value |= static_cast<uint32_t>(byte2) << 16;
	value |= static_cast<uint32_t>(byte1) << 8;
	value |= static_cast<uint32_t>(lowestByte0);
	return value;
}

/**
 * @brief Get bits 7 to 4 of a 8-bit value (the high nibble)
 *
 * @param byte The 8-bit input value
 * @return Bits 7 to 4 extracted and shifted to buts 3 to 0 as a byte (uint8_t)
 */
inline uint8_t u8_get_hi_nibble(const uint8_t byte) {
	return static_cast<uint8_t>(byte >> 4);
}

/**
 * @brief Get bits 3 to 0 of a 8-bit value (the low nibble)
 *
 * @param byte The 8-bit input value
 * @return Bits 3 to 0 extracted as a byte (uint8_t)
 */
inline uint8_t u8_get_lo_nibble(const uint8_t byte) {
	return static_cast<uint8_t>(byte & 0x0f);
}

/**
 * @brief Get bits 15 to 8 of a 16-bit value
 *
 * @param word The 16-bit input value
 * @return The result as a byte (uint8_t)
 */
inline uint8_t u16_get_hi_u8(const uint16_t word) {
	return static_cast<uint8_t>(word >> 8);
}

/**
 * @brief Get bits 7 to 0 of a 16-bit value
 *
 * @param word The 16-bit input value
 * @return The result as a byte (uint8_t)
 */
inline uint8_t u16_get_lo_u8(const uint16_t word) {
	return static_cast<uint8_t>(word & 0xFF);
}

/**
 * @brief Get bits 7 to 0 of a 32-bit value
 *
 * @param word32 The 32-bit input value
 * @return The result as a byte (uint8_t)
**/
inline uint8_t u32_get_byte0(const uint32_t word32) {
	return static_cast<uint8_t>(word32 & 0xFF);
}

/**
 * @brief Get bits 15 to 8 of a 32-bit value
 *
 * @param word32 The 32-bit input value
 * @return The result as a byte (uint8_t)
**/
inline uint8_t u32_get_byte1(const uint32_t word32) {
	return static_cast<uint8_t>((word32>>8) & 0xFF);
}

/**
 * @brief Get bits 23 to 16 of a 32-bit value
 *
 * @param word32 The 32-bit input value
 * @return The result as a byte (uint8_t)
**/
inline uint8_t u32_get_byte2(const uint32_t word32) {
	return static_cast<uint8_t>((word32>>16) & 0xFF);
}

/**
 * @brief Get bits 31 to 24 of a 32-bit value
 *
 * @param word32 The 32-bit input value
 * @return The result as a byte (uint8_t)
**/
inline uint8_t u32_get_byte3(const uint32_t word32) {
	return static_cast<uint8_t>((word32>>24) & 0xFF);
}

} // namespace NSEZSP
