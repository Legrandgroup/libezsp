/**
 * @file Logger.h
 * 
 * @brief Singleton logger
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "spi/ByteBuffer.h"

namespace NSSPI {
class ILogger;

typedef std::unique_ptr<ILogger, std::function<void(ILogger*)>> ILoggerInstance;

class Logger
{
public:
	/**
	 * @brief Singleton getter
	 * 
	 * @return A pointer to the singleton instance of the logger
	 */
	static ILogger *getInstance();

	/**
	 * @brief Utility function to dump a byte vector to a string
	 * 
	 * @param[in] input The vector container to dump
	 */
	static std::string byteSequenceToString(const std::vector<uint8_t>& input);

	/**
	 * @brief Utility function to dump bytes from a memory area to a string
	 * 
	 * @param[in] input The memory area to dump
	 * @param size The number of bytes to dump from the @p input buffer
	 */
	static std::string byteSequenceToString(const uint8_t* input, size_t size);

	/**
	 * @brief Utility function to dump a byte array to a string
	 * 
	 * @param[in] input The array container to dump
	 */
	template <std::size_t N>
	static std::string byteSequenceToString(const std::array<uint8_t, N>& input)
	{
		/* Note: because this is a member template-based method, it should be declare AND defined in the header file */
		std::ostringstream result;
	
		for(auto it=std::begin(input); it<std::end(input); it++) {
			if (result.tellp()>0) {
				result << " ";
			}
			result << Logger::byteToHexString(*it);
		}
		return result.str();
	}

	// /**
	//  * @brief Utility function to dump a byte buffer to a string
	//  * 
	//  * @param[in] The byte buffer container to dump
	//  */
	// // This method is commented-out because NSSPI::ByteBuffer is currently inheriting from std::vector<uint8_t> that is handled above
	// static std::string byteSequenceToString(const NSSPI::ByteBuffer& input);

private:
	Logger() = default;

	/**
	 * @brief Convert a byte to its 2-digit hexadecimal representation
	 * 
	 * @param byte The byte to represent
	 * @return A 2-character string contaning the hexadecimal representation of @p byte
	 */
	static std::string byteToHexString(uint8_t byte);

	static ILoggerInstance mInstance;
};

} // namespace NSSPI

#include "spi/ILogger.h"

#endif
