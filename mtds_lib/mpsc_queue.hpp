// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef LIBMTDS_MPSC_QUEUE_HPP
#define LIBMTDS_MPSC_QUEUE_HPP

#include <atomic>
#include <optional>
#include "details/tagged_ptr.hpp"

namespace mtds {

template<typename T>
class MpscQueue {
public:
    using value_type = T;
    using size_type = size_t;

    MpscQueue();
    ~MpscQueue();
    MpscQueue(const MpscQueue&) = delete;
    MpscQueue& operator=(const MpscQueue&) = delete;

    [[nodiscard]] bool empty() const;
    [[nodiscard]] size_type size() const { return m_size; }

    void clear() { while (try_dequeue().has_value()) continue; }
    template<typename U> void enqueue(U&& value);
    std::optional<T> try_dequeue();
    T dequeue();

    // Synonyms
    template<typename U> void push(U&& value) { enqueue(std::forward<U>(value)); }
    std::optional<T> try_pop() { return try_dequeue(); }
    T pop() { return dequeue(); }

private:
    using tagged_ptr = tp::tagged_ptr;
    using Node = tp::Node<T>;

    std::atomic<size_type> m_size = 0;
    std::atomic<tagged_ptr> m_head_ptr{};
    std::atomic<tagged_ptr> m_tail_ptr{};

    virtual void dispose_node(Node* node_ptr) { delete node_ptr; }
};

template<typename T>
MpscQueue<T>::MpscQueue() {
    auto dummy_node = tp::to_tagged_ptr(new Node{});
    m_head_ptr.store(dummy_node, std::memory_order_release);
    m_tail_ptr.store(dummy_node, std::memory_order_release);
}

template<typename T>
MpscQueue<T>::~MpscQueue() {
    clear();
    auto head = m_head_ptr.load(std::memory_order_relaxed);
    m_head_ptr.store(tp::tagged_nullptr, std::memory_order_relaxed);
    m_tail_ptr.store(tp::tagged_nullptr, std::memory_order_relaxed);
    delete tp::from_tagged_ptr<Node>(head);
}

template<typename T>
template<typename U>
void MpscQueue<T>::enqueue(U&& value) {
    auto new_node = tp::to_tagged_ptr(new Node{std::forward<U>(value)});
    tagged_ptr tail;

    while (true) {
        tail = m_tail_ptr.load(std::memory_order_relaxed);

        // Is tail consistent?
        if (tail != m_tail_ptr.load(std::memory_order_acquire)) { continue; }

        auto next = tp::from_tagged_ptr<Node>(tail)->next_ptr.load(
                std::memory_order_acquire);

        // Is tail still consistent?
        if (tail != m_tail_ptr) { continue; }

        // m_tail_ptr not pointing to the last node
        if (next != tp::tagged_nullptr) {
            // Try to swing m_tail_ptr to the next node
            m_tail_ptr.compare_exchange_weak( tail, tp::increment(next), std::memory_order_release );
            continue;
        }

        // m_tail_ptr was pointing to the last node
        auto tmp = tp::tagged_nullptr;

        // Try to link node at the end of the linked list
        if (tp::from_tagged_ptr<Node>(tail)->next_ptr.compare_exchange_strong(tmp, tp::combine_and_increment(new_node, next), std::memory_order_release )) { break; }
    }
    // Enqueue is done. Try to swing tail to the inserted node
    m_tail_ptr.compare_exchange_strong( tail, tp::combine_and_increment(new_node, tail), std::memory_order_acq_rel );
    ++m_size;
}

template<typename T>
std::optional<T> MpscQueue<T>::try_dequeue() {
    tagged_ptr head, next;
    while (true) {
        head = m_head_ptr.load(std::memory_order_acquire);
        next = tp::from_tagged_ptr<Node>(head)->next_ptr.load(std::memory_order_acquire);
        // Is head consistent?
        if (head != m_head_ptr.load(std::memory_order_acquire)) {
            continue;
        }
        // Is queue empty?
        if (next == tp::tagged_nullptr) {
            return {};
        }
        auto inc_next = tp::increment(next);
        auto tail = m_tail_ptr.load(std::memory_order_relaxed);
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
    dispose_node(tp::from_tagged_ptr<Node>(head));
    return tp::from_tagged_ptr<Node>(next)->value;
}

template<typename T>
T MpscQueue<T>::dequeue() {
    std::optional<T> temp;
    do {
        temp = try_dequeue();
    } while (!temp.has_value());
    return temp.value();
}

template<typename T>
bool MpscQueue<T>::empty() const {
    auto head = m_head_ptr.load(std::memory_order_acquire);
    auto next = tp::from_tagged_ptr<Node>(head)->next_ptr.load(std::memory_order_relaxed);
    return next == tp::tagged_nullptr;
}

}  // namespace mtds

#endif //LIBMTDS_MPSC_QUEUE_HPP
