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

#include <ezsp/export.h>
#include <ezsp/byte-manip.h>
#include <spi/ByteBuffer.h>

namespace NSSPI {
class ILogger;

typedef std::unique_ptr<ILogger, std::function<void(ILogger*)>> ILoggerInstance;

class LIBEXPORT Logger {
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
	 *
	 * @return The instance provided in @p input as its string representation
	 */
	static std::string byteSequenceToString(const std::vector<uint8_t>& input);

	/**
	 * @brief Utility function to dump bytes from a memory area to a string
	 *
	 * @param[in] input The memory area to dump
	 * @param size The number of bytes to dump from the @p input buffer
	 *
	 * @return The instance provided in @p input as its string representation
	 */
	static std::string byteSequenceToString(const uint8_t* input, size_t size);

	/**
	 * @brief Utility function to dump a byte array to a string
	 *
	 * @param[in] input The array container to dump
	 *
	 * @return The instance provided in @p input as its string representation
	 */
	template <std::size_t N>
	static std::string byteSequenceToString(const std::array<uint8_t, N>& input) {
		/* Note: because this is a member template-based method, it should be declare AND defined in the header file */
		std::ostringstream result;

		for(auto it=std::begin(input); it<std::end(input); it++) {
			if (result.tellp()>0) {
				result << " ";
			}
			result << NSEZSP::byteToHexString(*it);
		}
		return result.str();
	}

	/**
	 * @brief Utility function to dump a byte buffer to a string
	 *
	 * @param[in] input The byte buffer container to dump
	 *
	 * @return The instance provided in @p input as its string representation
	 */
	static std::string byteSequenceToString(const NSSPI::ByteBuffer& input);

private:
	Logger() = default;

	static ILoggerInstance mInstance;	/*!< The unique (singleton) logger instance */
};

} // namespace NSSPI

#endif
