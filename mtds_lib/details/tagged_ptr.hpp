// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MTDSLIB_TAGGED_PTR_HPP
#define MTDSLIB_TAGGED_PTR_HPP

#ifdef _MSC_VER
#define FORCE_INLINE __forceinline
#elif defined(__GNUC__)
#define FORCE_INLINE inline __attribute__((__always_inline__))
#elif defined(__clang__)
#if __has_attribute(__always_inline__)
#define FORCE_INLINE inline __attribute__((__always_inline__))
#else
#define FORCE_INLINE inline
#endif
#else
#define FORCE_INLINE inline
#endif

namespace mtds::tp {

using tagged_ptr = uintptr_t;

constexpr tagged_ptr tagged_nullptr = 0;

template<typename T>
struct Node {
    T value{};
    std::atomic<tagged_ptr> next_ptr = tagged_nullptr;
};

FORCE_INLINE tagged_ptr increment(tagged_ptr ptr) {
    auto inc_tag = ((0x3ffc & ptr >> 50) | (0b11 & ptr)) + 1;
    return ((0x3ffc & inc_tag) << 50) | (0b11 & inc_tag) | (0x000ffffffffffffc & ptr);
}

FORCE_INLINE tagged_ptr combine_and_increment(tagged_ptr ptr, tagged_ptr tag) {
    auto inc_tag = increment(tag);
    return (0x000ffffffffffffc & ptr) | (0xfff0000000000003 & inc_tag);
}

template<typename T>
tagged_ptr to_tagged_ptr(T ptr) {
    return reinterpret_cast<tagged_ptr>(ptr);
}

template<typename T>
T* from_tagged_ptr(tagged_ptr ptr) {
    return reinterpret_cast<T*>(0x000ffffffffffffc & ptr);
}

}  // namespace mtds::details

#undef FORCE_INLINE

#endif //MTDSLIB_TAGGED_PTR_HPP
