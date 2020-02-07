#ifndef __BYTEBUFFER_H__
#define __BYTEBUFFER_H__

#include <vector>

namespace NSSPI {

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
