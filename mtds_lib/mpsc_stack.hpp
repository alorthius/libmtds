// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef LIBMTDS_MPSC_STACK_HPP
#define LIBMTDS_MPSC_STACK_HPP

#include <atomic>
#include <optional>
#include "details/tagged_ptr.hpp"

namespace mtds {

template<typename T>
class MpscStack {
public:
    using value_type = T;
    using size_type = size_t;

    MpscStack() = default;
    ~MpscStack() { clear(); }
    MpscStack(const MpscStack&) = delete;
    MpscStack& operator=(const MpscStack&) = delete;

    [[nodiscard]] bool empty() const {
        return tp::from_tagged_ptr<Node>(m_top_ptr.load(std::memory_order_relaxed)) == nullptr;
    }
    [[nodiscard]] size_type size() const { return m_size; }

    void clear() { while (try_pop().has_value()) continue; }
    template<typename U> void push(U&& value);
    std::optional<T> try_pop();
    T pop();

private:
    using tagged_ptr = tp::tagged_ptr;
    using Node = tp::Node<T>;

    std::atomic<size_type> m_size = 0;
    std::atomic<tagged_ptr> m_top_ptr = tp::tagged_nullptr;

    virtual void dispose_node(Node* node_ptr) { delete node_ptr; }
};

template<typename T>
template<typename U>
void MpscStack<T>::push(U &&value) {
    auto new_node = tp::to_tagged_ptr( new Node{std::forward<U>(value)} );
    auto top = m_top_ptr.load(std::memory_order_relaxed);

    while (true) {
        tp::from_tagged_ptr<Node>(new_node)->next_ptr.store( top, std::memory_order_relaxed );
        if (m_top_ptr.compare_exchange_weak( top, tp::combine_and_increment(new_node, top),
                                             std::memory_order_release, std::memory_order_acquire )) {
            ++m_size;
            return;
        }
    }
}

template<typename T>
std::optional<T> MpscStack<T>::try_pop() {
    while (true) {
        auto top = m_top_ptr.load(std::memory_order_acquire);
        // Is stack empty?
        if (tp::from_tagged_ptr<Node>(top) == nullptr) {
            return {};
        }
        auto next = tp::from_tagged_ptr<Node>(top)->next_ptr.load(std::memory_order_relaxed);
        // Try to swing m_top_ptr to the next node
        if (m_top_ptr.compare_exchange_weak(top, tp::combine_and_increment(next, top),
                                            std::memory_order_acquire, std::memory_order_relaxed)) {
            --m_size;
            auto value = tp::from_tagged_ptr<Node>(top)->value;
            dispose_node(tp::from_tagged_ptr<Node>(top));
            return value;
        }
    }
}

template<typename T>
T MpscStack<T>::pop() {
    std::optional<T> temp;
    do {
        temp = try_pop();
    } while (!temp.has_value());
    return *temp;
}

}  // namespace mtds

#endif //LIBMTDS_MPSC_STACK_HPP