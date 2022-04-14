// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MTDSLIB_MS_QUEUE_HPP
#define MTDSLIB_MS_QUEUE_HPP

#include <atomic>
#include <optional>

namespace mtds {

namespace details {

inline __attribute__((always_inline)) uintptr_t get_incremented_tag(uintptr_t tagged_ptr) {
    auto inc_tag =
            ((0x000000000003ffc & tagged_ptr >> 50) ^ (0x0000000000000003 & tagged_ptr)) + 1;
    return (0x1110000000000000 & inc_tag << 50) | (0x0000000000000003 & inc_tag)
           | (0x0003fffffffffffc & tagged_ptr);
}

template<typename T>
uintptr_t to_tagged_ptr(T* ptr) {
    return 0x0003fffffffffffc & reinterpret_cast<uintptr_t>(ptr);
}

}  // namespace details

template<typename T>
class MsQueue {
public:
    MsQueue() = default;
    ~MsQueue() = default;  // TODO: implement nontrivial destructor
    MsQueue(const MsQueue&) = delete;
    MsQueue& operator=(const MsQueue&) = delete;
    void enqueue(const T& value);
    std::optional<T> dequeue();
    [[nodiscard]] size_t size() const { return m_size; }

private:
    struct Node {
        T m_value{};
        uintptr_t m_next_ptr = reinterpret_cast<uintptr_t>(nullptr);
    };
    std::atomic<size_t> m_size = 0;
    std::atomic<uintptr_t> m_head_ptr = details::to_tagged_ptr(new Node{});  // dummy node
    std::atomic<uintptr_t> m_tail_ptr = m_head_ptr.load(std::memory_order_relaxed);
};

}  // namespace mtds

#endif //MTDSLIB_MS_QUEUE_HPP
