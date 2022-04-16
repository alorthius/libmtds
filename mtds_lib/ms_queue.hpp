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

template<typename T>
T* from_tagged_ptr(uintptr_t tagged_ptr) {
    return reinterpret_cast<T*>(0x0003fffffffffffc & tagged_ptr);
}

}  // namespace details

template<typename T>
class MsQueue {
public:
    MsQueue();
    ~MsQueue() = default;  // TODO: implement nontrivial destructor
    MsQueue(const MsQueue&) = delete;
    MsQueue& operator=(const MsQueue&) = delete;
    void enqueue(const T& value);
    std::optional<T> dequeue();
    [[nodiscard]] size_t size() const { return m_size; }

private:
    struct Node {
        std::atomic<uintptr_t> m_next_ptr = reinterpret_cast<uintptr_t>(nullptr);
        T m_value{};
    };
    std::atomic<size_t> m_size = 0;
    std::atomic<uintptr_t> m_head_ptr{};
    std::atomic<uintptr_t> m_tail_ptr{};
};

template<typename T>
MsQueue<T>::MsQueue() {
    auto dummy_node = details::to_tagged_ptr(new Node{});
    m_head_ptr.store(dummy_node, std::memory_order_release);
    m_tail_ptr.store(dummy_node, std::memory_order_release);
}

template<typename T>
std::optional<T> MsQueue<T>::dequeue() {
    uintptr_t head;
    while (true) {
        head = m_head_ptr.load(std::memory_order_relaxed);
        // Is head consistent?
        if (head != m_head_ptr.load(std::memory_order_acquire)) {
            continue;
        }
        auto tail = m_tail_ptr.load(std::memory_order_relaxed);
        auto next = details::from_tagged_ptr<Node>(head)->m_next_ptr.load(
                std::memory_order_acquire);
        // Is head still consistent?
        if (head != m_head_ptr.load(std::memory_order_relaxed)) {
            continue;
        }
        // Is queue empty?
        if (next == reinterpret_cast<uintptr_t>(nullptr)) {
            return {};
        }
        // Is m_tail_ptr falling behind?
        if (head == tail) {
            m_tail_ptr.compare_exchange_strong(tail, next, std::memory_order_release);
            continue;
        }
        // Try to swing m_head_ptr to the next node
        if (m_head_ptr.compare_exchange_strong(head, next, std::memory_order_release)) {
            break;
        }
    }
    --m_size;
    // TODO: implement memory disposal
    return details::from_tagged_ptr<Node>(head)->value;
}

}  // namespace mtds

#endif //MTDSLIB_MS_QUEUE_HPP
