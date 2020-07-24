/**
 * @file ByteBuffer.h
 *
 * @brief Byte buffer manipulation class
 *
 * @warning This class derives from std::vector, which has many dangerous implications, mainly that no dynamically allocated member should be allowed, because std::vector's destructor is not virtual.
 */

#ifndef __BYTEBUFFER_H__
#define __BYTEBUFFER_H__

#include <vector>
#include <array>
#include <cstddef> // For size_t

#include <ezsp/export.h>
#include <ezsp/byte-manip.h>

namespace NSSPI {

/*
 * FIXME: This class would be better rewritten as encapsulating std::vector
 * Although this can be tedious, in this library, we don't use the full std::vector interface on ByteBuffer.
 */
class LIBEXPORT ByteBuffer : public std::vector<uint8_t> {
	typedef std::vector<uint8_t> _Base;

public:
	ByteBuffer() : _Base() { }
	ByteBuffer(const ByteBuffer& other) : _Base(other) { } //NOSONAR
	/**
	 * @brief Destructor
	 *
	 * @warning This destructor definition is here to please SonarCloud because std::vector class's destructor is not virtual anyway
	 */
	~ByteBuffer() = default;

#if __cplusplus >= 201103L
	ByteBuffer(ByteBuffer&& other) noexcept : _Base(std::move(other)) { }
	ByteBuffer(std::vector<uint8_t>&& other) noexcept : _Base(std::move(other)) { }
	ByteBuffer(const std::initializer_list<uint8_t>& list) : _Base(list) { }
#endif
	template<typename _InputIterator,
	         typename = std::_RequireInputIter<_InputIterator>>
	ByteBuffer(_InputIterator __first, _InputIterator __last) : _Base(__first, __last) { }

	/**
	 * @brief Construct from a buffer in memory
	 *
	 * @param[in] fromBuf A pointer to the memory buffer to use as input
	 * @param size The size in bytes to read from @p fromBuf
	 */
	ByteBuffer(const uint8_t* fromBuf, size_t size) : _Base() {
		while (size--) {
			this->push_back(*fromBuf++);
		}
	}
	/**
	 * @brief Append another ByteBuffer's content to our current content
	 *
	 * @param[in] other The array containing the content to add
	 */
	template <std::size_t N>
	ByteBuffer(const std::array<uint8_t, N>& other) : _Base() {
		/* Note: because this is a member template-based method, it should be declare AND defined in the header file */
		for (auto it=other.begin(); it<other.end(); it++) {
			this->push_back(*it);
		}
	}

	ByteBuffer& operator=(const ByteBuffer& other) {
		if (&other != this) {
			_Base::operator=(static_cast<_Base>(other));
		}
		return *this;
	}
#if __cplusplus >= 201103L
	ByteBuffer& operator=(_Base&& other) noexcept {
		_Base::operator=(std::move(other));
		return *this;
	}
	ByteBuffer& operator=(const std::initializer_list<uint8_t>& list) {
		_Base::operator=(list);
		return *this;
	}
#endif
	/**
	 * @brief Append another ByteBuffer's content to our current content
	 *
	 * @param[in] other The ByteBuffer containing the content to add
	 */
	void append(const ByteBuffer& other) {
		for (auto it=other.begin(); it<other.end(); it++) {
			this->push_back(*it);
		}
	}
	/**
	 * @brief Append another ByteBuffer's content to our current content
	 *
	 * @param[in] other The array containing the content to add
	 */
	template <std::size_t N>
	void append(const std::array<uint8_t, N>& other) {
		/* Note: because this is a member template-based method, it should be declare AND defined in the header file */
		for (auto it=other.begin(); it<other.end(); it++) {
			this->push_back(*it);
		}
	}
	/**
	 * @brief Append another ByteBuffer's content to our current content
	 *
	 * @param[in] other The ByteBuffer containing the content to add
	 */
	void append(ByteBuffer&& other) {
		for (auto it=other.begin(); it<other.end(); it++) {
			this->push_back(std::move(*it));
		}
	}
	/**
	 * @brief Write the content of this instance into a memory buffer
	 *
	 * @param[in] dst A pointer to the beginning of the memory buffer
	 *
	 * @warning The caller must make sure that we will have enough room in dst to store all bytes (it should be able to know this by consulting our size() method)
	 */
	void toMemory(uint8_t* dst) const {
		for (auto it=this->begin(); it<this->end(); it++) {
			*dst++ = *it;
		}
	}

	/**
	 * @brief Serialize to an iostream
	 *
	 * @param out The original output stream
	 * @param data The object to serialize
	 *
	 * @return The new output stream with serialized data appended
	 */
	friend std::ostream& operator<<(std::ostream& out, const ByteBuffer& data) {
		/* As long as ByteBuffer inherits from vector<uint8_t>, conversion to a string could also be delegated to Logger::byteSequenceToString, like this:
		   out << NSSPI::Logger::byteSequenceToString(data);
		   But some gcc version do not handle this properly and issue a weird "error: ‘NSSPI::Logger’ has not been declared" even with the proper #include <spi/Logger.h>
		   We'll thus stick to implementing the parsing manually below */
		for (auto it=std::begin(data); it<std::end(data); it++) {
			if (it != std::begin(data)) {
				out << std::string(" ");
			}
			out << NSEZSP::byteToHexString(*it);
		}
		return out;
	}
};
} // namespace NSSPI

#endif
