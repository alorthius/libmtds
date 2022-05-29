// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MTDS_TREIBER_STACK_HPP
#define MTDS_TREIBER_STACK_HPP

#include <atomic>
#include <optional>
#include <thread>
#include "details/tagged_ptr.hpp"
#include "details/backoff.hpp"

namespace mtds {

template<typename T, typename Backoff = backoff::exp_backoff>
class TreiberStack {
public:
    using value_type = T;
    using size_type = size_t;

    TreiberStack() = default;
    ~TreiberStack() { clear(); }
    TreiberStack(const TreiberStack&) = delete;
    TreiberStack& operator=(const TreiberStack&) = delete;

    [[nodiscard]] bool empty() const {
        return m_top_ptr.load(std::memory_order_relaxed).ptr() == nullptr;
    }
    [[nodiscard]] size_type size() const { return m_size; }

    void clear() { while (try_pop()) continue; }
    template<typename U> void push(U&& value);
    std::optional<T> try_pop();
    T pop();

private:
    using Node = tagged_ptr::Node<T>;
    using TaggedPtr = tagged_ptr::TaggedPtr<Node>;

    std::atomic<size_type> m_size = 0;
    std::atomic<TaggedPtr> m_top_ptr{};
};

template<typename T, typename Backoff>
template<typename U>
void TreiberStack<T, Backoff>::push(U &&value) {
    Backoff backoff;

    TaggedPtr new_node{new Node{std::forward<U>(value)}};
    auto top = m_top_ptr.load(std::memory_order_relaxed);

    while (true) {
        new_node.ptr()->next_ptr.store( top, std::memory_order_relaxed );
        if (m_top_ptr.compare_exchange_weak( top, TaggedPtr{new_node.ptr(), (top.tag() + 1) % (1 << 14)},
                                             std::memory_order_release, std::memory_order_acquire )) {
            ++m_size;
            return;
        }
        backoff();
    }
}

template<typename T, typename Backoff>
std::optional<T> TreiberStack<T, Backoff>::try_pop() {
    Backoff backoff;

    while (true) {
        auto top = m_top_ptr.load(std::memory_order_acquire);

        // Is stack empty?
        if (top.ptr() == nullptr) {
            return {};
        }
        auto next = top.ptr()->next_ptr.load(std::memory_order_relaxed);

        // Try to swing m_top_ptr to the next node
        if (m_top_ptr.compare_exchange_weak(top, TaggedPtr{next.ptr(), (top.tag() + 1) % (1 << 14)},
                                            std::memory_order_acquire, std::memory_order_relaxed)) {
            --m_size;
            auto value = top.ptr()->value;
            delete top.ptr();
            return value;
        }
        backoff();
    }
}

template<typename T, typename Backoff>
T TreiberStack<T, Backoff>::pop() {
    std::optional<T> temp;
    do {
        temp = try_pop();
    } while (!temp.has_value());

    return *temp;
}

}  // namespace mtds

#endif //MTDS_TREIBER_STACK_HPP
