// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MTDSLIB_MS_QUEUE_HPP
#define MTDSLIB_MS_QUEUE_HPP

#include <atomic>
#include <optional>

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

namespace mtds {

namespace details {

FORCE_INLINE uintptr_t increment(uintptr_t tagged_ptr) {
    auto inc_tag =
            ((0x3ffc & tagged_ptr >> 50) ^ (0x3 & tagged_ptr)) + 1;
    return ((0x3ffc & inc_tag) << 50) | (0x3 & inc_tag)
           | (0xffffffffffffc & tagged_ptr);
}

FORCE_INLINE uintptr_t combine_and_increment(uintptr_t ptr, uintptr_t tag) {
    auto inc_tag = increment(tag);
    return (0xffffffffffffc & ptr) | (0xfff0000000000003 & inc_tag);
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
        auto inc_next = details::increment(next);
        // Is m_tail_ptr falling behind?
        if (head == tail) {
            m_tail_ptr.compare_exchange_strong(tail, inc_next, std::memory_order_release);
            continue;
        }
        // Try to swing m_head_ptr to the next node
        if (m_head_ptr.compare_exchange_strong(head, inc_next, std::memory_order_release)) {
            break;
        }
    }
    --m_size;
    // TODO: implement memory disposal
    return details::from_tagged_ptr<Node>(head)->value;
}

template<typename T>
void MsQueue<T>::enqueue(const T &value) {
    // TODO: extract node from freelist
    auto new_node = details::to_tagged_ptr(new Node{value});
    uintptr_t tail;

    while (true) {
        tail = m_tail_ptr.load(std::memory_order_relaxed);

        // Is tail consistent?
        if (tail != m_tail_ptr.load(std::memory_order_acquire)) { continue; }

        auto next = details::from_tagged_ptr<Node>(tail)->m_next_ptr.load(
                std::memory_order_acquire);

        // Is tail still consistent?
        if (tail != m_tail_ptr) { continue; }

        // m_tail_ptr not pointing to the last node
        if (next != reinterpret_cast<uintptr_t>(nullptr)) {
            // Try to swing m_tail_ptr to the next node
            m_tail_ptr.compare_exchange_weak( tail, details::increment(next), std::memory_order_release );
            continue;
        }

        // m_tail_ptr was pointing to the last node
        auto tmp = reinterpret_cast<uintptr_t>(nullptr);

        // Try to link node at the end of the linked list
        if (details::from_tagged_ptr<Node>(tail)->m_next_ptr.compare_exchange_strong( tmp, details::combine_and_increment(new_node, next), std::memory_order_release )) { break; }
    }
    // Enqueue is done. Try to swing tail to the inserted node
    m_tail_ptr.compare_exchange_strong( tail, details::combine_and_increment(new_node, tail), std::memory_order_acq_rel );
    ++m_size;
}

}  // namespace mtds

#endif //MTDSLIB_MS_QUEUE_HPP
