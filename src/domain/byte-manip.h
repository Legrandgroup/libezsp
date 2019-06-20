/**
 * @brief Utility function to create unsigned 16-bit integers from their 8-bit high and low bytes and vice versa
 */

#pragma once

#include <cstdint>

inline uint16_t dble_u8_to_u16(const uint8_t highByte, const uint8_t lowByte) {
	return static_cast<uint16_t>( static_cast<uint16_t>(highByte<<8) | static_cast<uint16_t>(lowByte) );
}

inline uint32_t quad_u8_to_u32(const uint8_t highestByte3, const uint8_t byte2, const uint8_t byte1, const uint8_t lowestByte0) {
	return static_cast<uint32_t>( static_cast<uint32_t>(highestByte3<<24) |
	                              static_cast<uint32_t>(byte2<<16) |
	                              static_cast<uint32_t>(byte1<<16) |
	                              static_cast<uint32_t>(lowestByte0) );
}

inline uint8_t u16_get_hi_u8(const uint16_t word) {
	return static_cast<uint8_t>(word >> 8);
}

inline uint8_t u16_get_lo_u8(const uint16_t word) {
	return static_cast<uint8_t>(word & 0xFF);
}
