// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MTDS_TAGGED_PTR_HPP
#define MTDS_TAGGED_PTR_HPP

#include <cmath>
#include <stdexcept>

#ifdef _MSC_VER
#define FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define FORCE_INLINE inline __attribute__((__always_inline__))
#else
#define FORCE_INLINE inline
#endif

namespace mtds::tagged_ptr {

/*
 * 12 upper and 2 lower bits of a 64-bit pointer are used to store the tag which is incremented
 * every successful CAS operation
 */
template<typename T>
class TaggedPtr {
public:
    FORCE_INLINE TaggedPtr() = default;

    FORCE_INLINE explicit TaggedPtr(T* ptr) {
        m_ptr = reinterpret_cast<uintptr_t>(ptr);
    }

    FORCE_INLINE TaggedPtr(T* ptr, unsigned tag) {
        if (tag >= std::pow(2, 14)) {  // Tag is 14-bit
            throw std::overflow_error{"Tag overflow"};
        }
        m_ptr = (0x000ffffffffffffc & reinterpret_cast<uintptr_t>(ptr))
                | (0xfff0000000000003U & tag);
    }

    FORCE_INLINE T* ptr() {
        return reinterpret_cast<T*>(0x000ffffffffffffc & m_ptr);
    }

    FORCE_INLINE unsigned tag() {
        return (0x3ffc & m_ptr >> 50) | (0b11 & m_ptr);
    }

    template<typename U>
    FORCE_INLINE bool operator==(const TaggedPtr<U>& rhs) {
        return m_ptr == rhs.m_ptr;
    }

    template<typename U>
    FORCE_INLINE bool operator!=(const TaggedPtr<U>& rhs) {
        return !(*this == rhs);
    }

private:
    uintptr_t m_ptr = reinterpret_cast<uintptr_t>(nullptr);
};

template<typename T>
struct Node {
    T value{};
    std::atomic<TaggedPtr<Node<T>>> next_ptr{};
};

}  // namespace mtds::details

#undef FORCE_INLINE

#endif //MTDS_TAGGED_PTR_HPP
