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

namespace NSSPI {

/*
 * FIXME: This class would be better rewritten as encapsulating std::vector
 * Although this can be tedious, in this library, we don't use the full std::vector interface on ByteBuffer.
 */
class ByteBuffer : public std::vector<uint8_t>
{
    typedef std::vector<uint8_t> _Base;

public:
    ByteBuffer() : _Base() { }
    ByteBuffer(const ByteBuffer& other) : _Base(other) { }
#if __cplusplus >= 201103L
    ByteBuffer(ByteBuffer&& other) noexcept : _Base(std::move(other)) { }
    ByteBuffer(std::vector<uint8_t>&& other) noexcept : _Base(std::move(other)) { }
    ByteBuffer(const std::initializer_list<uint8_t>& list) : _Base(list) { }
#endif
    template<typename _InputIterator,
            typename = std::_RequireInputIter<_InputIterator>>
    ByteBuffer(_InputIterator __first, _InputIterator __last) : _Base(__first, __last) { }
    ByteBuffer& operator=(const ByteBuffer& other) {
        return static_cast<ByteBuffer&>(_Base::operator=(static_cast<_Base>(other)));
    }
#if __cplusplus >= 201103L
    ByteBuffer& operator=(_Base&& other) noexcept {
        return static_cast<ByteBuffer&>(_Base::operator=(std::move(other)));
    }
    ByteBuffer& operator=(const std::initializer_list<uint8_t>& list) {
        return static_cast<ByteBuffer&>(_Base::operator=(list));
    }
#endif
};
} // namespace NSSPI

#endif
