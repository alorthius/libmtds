// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MTDS_TAGGED_PTR_HPP
#define MTDS_TAGGED_PTR_HPP

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
 * every successful CAS operation.
 *
 * In modern systems, 52 bits are enough to address memory, and addresses are multiples of 4 due to
 * data alignment. We use the unused 14 bits to store the tag in the same 64-bit value to be able to
 * use the CAS. x86_64 has no support for a double-word CAS, and we are forced to use this hack, as
 * tagged pointers operate on 2 values for the tag and pointer.
 */
template<typename T>
class TaggedPtr {
public:
    FORCE_INLINE TaggedPtr() = default;

    FORCE_INLINE explicit TaggedPtr(T* ptr) {
        m_ptr = reinterpret_cast<uintptr_t>(ptr);
    }

    FORCE_INLINE TaggedPtr(T* ptr, unsigned tag) {
        if (tag >= 2 << 14) {  // Tag is 14-bit
            throw std::overflow_error{"Tag overflow"};
        }
        // Store the pointer in bits [3, 52] and the tag in bits [1, 2] and [52, 64], splitting it
        // into two parts
        m_ptr = (0x000ffffffffffffc & reinterpret_cast<uintptr_t>(ptr))
                | ((0x3ffc & static_cast<unsigned long long>(tag)) << 50) | (0b11 & tag);
    }

    FORCE_INLINE T* ptr() {
        // Get bits [3, 52] where the pointer is stored
        return reinterpret_cast<T*>(0x000ffffffffffffc & m_ptr);
    }

    FORCE_INLINE unsigned tag() {
        // Get bits [1, 2] and [52, 64] where the tag is stored and combine the split parts
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
