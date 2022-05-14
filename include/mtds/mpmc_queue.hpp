// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MTDS_MPMC_QUEUE_HPP
#define MTDS_MPMC_QUEUE_HPP

#include <atomic>
#include <optional>
#include <thread>
#include "details/tagged_ptr.hpp"
#include "details/backoff_strategy.hpp"

namespace mtds {

template<typename T, typename backoffType = bos::exp_backoff>
class MpmcQueue {
public:
    using value_type = T;
    using size_type = size_t;

    MpmcQueue();
    ~MpmcQueue();
    MpmcQueue(const MpmcQueue&) = delete;
    MpmcQueue& operator=(const MpmcQueue&) = delete;

    [[nodiscard]] bool empty() const;
    [[nodiscard]] size_type size() const { return m_size; }

    void clear() { while (try_dequeue()) continue; }
    template<typename U> void enqueue(U&& value);
    std::optional<T> try_dequeue();
    T dequeue();

    // Synonyms
    template<typename U> void push(U&& value) { enqueue(std::forward<U>(value)); }
    std::optional<T> try_pop() { return try_dequeue(); }
    T pop() { return dequeue(); }

private:
    using Node = tagged_ptr::Node<T>;
    using TaggedPtr = tagged_ptr::TaggedPtr<Node>;

    std::atomic<size_type> m_size = 0;
    std::atomic<TaggedPtr> m_head_ptr{};
    std::atomic<TaggedPtr> m_tail_ptr{};
};

template<typename T, typename backoffType>
MpmcQueue<T, backoffType>::MpmcQueue() {
    TaggedPtr dummy_node{new Node{}};
    m_head_ptr.store(dummy_node, std::memory_order_release);
    m_tail_ptr.store(dummy_node, std::memory_order_release);
}

template<typename T, typename backoffType>
MpmcQueue<T, backoffType>::~MpmcQueue() {
    clear();
    auto head = m_head_ptr.load(std::memory_order_relaxed);
    m_head_ptr.store(TaggedPtr{}, std::memory_order_relaxed);
    m_tail_ptr.store(TaggedPtr{}, std::memory_order_relaxed);
    delete head.ptr();
}

template<typename T, typename backoffType>
template<typename U>
void MpmcQueue<T, backoffType>::enqueue(U&& value) {
    backoffType backoff;
    TaggedPtr new_node{new Node{std::forward<U>(value)}};
    TaggedPtr tail;

    while (true) {
        tail = m_tail_ptr.load(std::memory_order_relaxed);

        // Is tail consistent?
        if (tail != m_tail_ptr.load(std::memory_order_acquire)) { continue; }

        auto next = tail.ptr()->next_ptr.load(
                std::memory_order_acquire);

        // m_tail_ptr not pointing to the last node
        if (next.ptr() != nullptr) {
            // Try to swing m_tail_ptr to the next node
            m_tail_ptr.compare_exchange_weak( tail, TaggedPtr{next.ptr(), tail.tag() + 1}, std::memory_order_release );
            continue;
        }

        // m_tail_ptr was pointing to the last node
        TaggedPtr tmp;

        // Try to link node at the end of the linked list
        if (tail.ptr()->next_ptr.compare_exchange_strong(tmp, TaggedPtr{new_node.ptr(), next.tag() + 1}, std::memory_order_release )) { break; }

        backoff();
    }
    // Enqueue is done. Try to swing tail to the inserted node
    m_tail_ptr.compare_exchange_strong(tail, TaggedPtr{new_node.ptr(), tail.tag() + 1}, std::memory_order_acq_rel );
    ++m_size;
}

template<typename T, typename backoffType>
std::optional<T> MpmcQueue<T, backoffType>::try_dequeue() {
    backoffType backoff;
    TaggedPtr head, next;
    T value;
    while (true) {
        head = m_head_ptr.load(std::memory_order_acquire);
        next = head.ptr()->next_ptr.load(std::memory_order_acquire);
        // Is head consistent?
        if (head != m_head_ptr.load(std::memory_order_acquire)) {
            continue;
        }
        // Is queue empty?
        if (next.ptr() == nullptr) {
            return {};
        }
        auto tail = m_tail_ptr.load(std::memory_order_relaxed);
        // Is m_tail_ptr falling behind?
        if (head == tail) {
            m_tail_ptr.compare_exchange_strong(tail, TaggedPtr{next.ptr(), tail.tag() + 1},
                                               std::memory_order_release);
            continue;
        }
        // Read value before CAS, otherwise another dequeue might free the next node
        value = next.ptr()->value;
        // Try to swing m_head_ptr to the next node
        if (m_head_ptr.compare_exchange_strong(head, TaggedPtr{next.ptr(), head.tag() + 1},
                                               std::memory_order_release)) {
            break;
        }
        backoff();
    }
    --m_size;
    delete head.ptr();
    return value;
}

template<typename T, typename backoffType>
T MpmcQueue<T, backoffType>::dequeue() {
    std::optional<T> temp;
    do {
        temp = try_dequeue();
    } while (!temp);
    return *temp;
}

template<typename T, typename backoffType>
bool MpmcQueue<T, backoffType>::empty() const {
    auto head = m_head_ptr.load(std::memory_order_acquire);
    auto next = head.ptr()->next_ptr.load(std::memory_order_relaxed);
    return next.ptr() == nullptr;
}

}  // namespace mtds

#endif //MTDS_MPMC_QUEUE_HPP
