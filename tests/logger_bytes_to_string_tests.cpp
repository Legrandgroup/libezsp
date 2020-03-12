#include <iostream>
#include <iomanip>
#include <vector>
#include <array>

#include "spi/ILogger.h"
#include "TestHarness.h"

TEST_GROUP(logger_bytes_to_string_tests) {
};

TEST(logger_bytes_to_string_tests, std_vector_to_string) {
	std::vector<uint8_t> vec;
	vec.push_back(0x55);
	vec.push_back(0x0f);
	vec.push_back(0x3a);
	vec.push_back(0xe0);
	vec.push_back(0x00);

	std::string result = NSSPI::Logger::byteSequenceToString(vec);
	if (result != "55 0f 3a e0 00") {
		FAILF("Failed converting a vector to string: \"%s\"", result.c_str());
	}
	NOTIFYPASS();
}

TEST(logger_bytes_to_string_tests, empty_std_vector_to_string) {
	std::vector<uint8_t> vec;

	if (NSSPI::Logger::byteSequenceToString(vec) != "") {
		FAILF("Failed converting a vector to string");
	}
	NOTIFYPASS();
}

TEST(logger_bytes_to_string_tests, std_array_to_string) {
	std::array<uint8_t, 5> arr;
	arr.at(0) = 0x55;
	arr.at(1) = 0x0f;
	arr.at(2) = 0x3a;
	arr.at(3) = 0xe0;
	arr.at(4) = 0x00;

	std::string result = NSSPI::Logger::byteSequenceToString(arr);
	if (result != "55 0f 3a e0 00") {
		FAILF("Failed converting an array to string: \"%s\"", result.c_str());
	}
	NOTIFYPASS();
}

TEST(logger_bytes_to_string_tests, bytebuffer_to_string) {
	NSSPI::ByteBuffer buf({ 0x55, 0x0f, 0x3a, 0xe0, 0x00 });

	std::string result = NSSPI::Logger::byteSequenceToString(buf);
	if (result != "55 0f 3a e0 00") {
		FAILF("Failed converting an array to string: \"%s\"", result.c_str());
	}
	NOTIFYPASS();
}

#ifndef USE_CPPUTEST
void unit_tests_logger_bytes_to_string() {
	std_vector_to_string();
	empty_std_vector_to_string();
	std_array_to_string();
}
#endif	// USE_CPPUTEST
