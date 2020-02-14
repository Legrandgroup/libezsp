/**
 * @file Logger.cpp
 * 
 * @brief Singleton logger
 */

#include <ezsp/byte-manip.h>

#include "spi/Logger.h"
#ifdef USE_RARITAN
#include "spi/raritan/RaritanLogger.h"
namespace NSSPI {
typedef RaritanLogger LoggerInstance;
}
#endif
#ifdef USE_CPPTHREADS
#include "spi/console/ConsoleLogger.h"
namespace NSSPI {
typedef ConsoleLogger LoggerInstance;
}
#endif

using NSSPI::Logger;
using NSSPI::ILogger;

NSSPI::ILoggerInstance Logger::mInstance;

ILogger *Logger::getInstance()
{
	static NSSPI::LoggerInstance logger;

	static bool static_init = []()->bool {
		mInstance = NSSPI::ILoggerInstance(&logger, [](ILogger* ptr)
        {
        });
		return true;
	}();
	return mInstance.get();
}

std::string Logger::byteToHexString(uint8_t byte)
{
	std::string result("00");
	uint8_t nibble = NSEZSP::u8_get_hi_nibble(byte);
	if (nibble>=0 && nibble<=9) {
		result[0] = nibble + '0';
	}
	else {
		result[0] = nibble - 0x0a + 'a';
	}
	nibble = NSEZSP::u8_get_lo_nibble(byte);
	if (nibble>=0 && nibble<=9) {
		result[1] = nibble + '0';
	}
	else {
		result[1] = nibble - 0x0a + 'a';
	}
	return result;
}

std::string Logger::byteSequenceToString(const std::vector<uint8_t>& input)
{
	std::ostringstream result;

	for(auto it=std::begin(input); it<std::end(input); it++) {
		if (result.tellp()>0) {
			result << " ";
		}
		result << Logger::byteToHexString(*it);
	}
	return result.str();
}

std::string Logger::byteSequenceToString(const uint8_t* input, size_t size)
{

	std::ostringstream result;

	for (unsigned int i = 0; i<size; i++) {
		if (i != 0) {
			result << " ";
		}
		result << Logger::byteToHexString(input[i]);
	}
	return result.str();
}

/*
// This method is commented-out because NSSPI::ByteBuffer is currently inheriting from std::vector<uint8_t> that is handled above
std::string Logger::byteSequenceToString(const NSSPI::ByteBuffer& input)
{
	std::ostringstream result;

	for(auto it=std::begin(input); it<std::end(input); it++) {
		if (result.tellp()>0) {
			result << " ";
		}
		result << Logger::byteToHexString(*it);
	}
	return result.str();
}
*/
