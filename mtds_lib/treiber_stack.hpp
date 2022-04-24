// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef LIBMTDS_TREIBER_STACK_HPP
#define LIBMTDS_TREIBER_STACK_HPP

#include <atomic>
#include <optional>
#include "details/tagged_ptr.hpp"

namespace mtds {

template<typename T>
class TreiberStack {
public:
    using value_type = T;
    using size_type = size_t;
    using tagged_ptr = tp::tagged_ptr;

    TreiberStack() = default;
    ~TreiberStack() { clear(); }
    TreiberStack(const TreiberStack&) = delete;
    TreiberStack& operator=(const TreiberStack&) = delete;

    [[nodiscard]] bool empty() const {
        return m_top_ptr.load(std::memory_order_relaxed) == tp::tagged_nullptr;
    }
    [[nodiscard]] size_type size() const { return m_size; }

    void clear() { while (try_pop().has_value()) continue; }
    template<typename U>
    void push(U&& value);
    std::optional<T> try_pop();
    T pop();

private:
    struct Node {
        T value{};
        std::atomic<tagged_ptr> next_ptr = tp::tagged_nullptr;
    };
    std::atomic<size_type> m_size = 0;
    std::atomic<tagged_ptr> m_top_ptr = tp::tagged_nullptr;
};

template<typename T>
std::optional<T> TreiberStack<T>::try_pop() {
    while (true) {
        auto top = m_top_ptr.load(std::memory_order_acquire);
        // Is stack empty?
        if (top == tp::tagged_nullptr) {
            return {};
        }
        auto next = tp::from_tagged_ptr<Node>(top)->next_ptr.load(std::memory_order_relaxed);
        if (m_top_ptr.compare_exchange_weak(top, tp::combine_and_increment(next, top),
                                            std::memory_order_acquire, std::memory_order_relaxed)) {
            --m_size;
            return tp::from_tagged_ptr<Node>(top)->value;
        }
    }
}

template<typename T>
T TreiberStack<T>::pop() {
    std::optional<T> temp;
    do {
        temp = try_pop();
    } while (!temp.has_value());
    return temp.value();
}

template<typename T>
template<typename U>
void TreiberStack<T>::push(U &&value) {
    auto new_node = tp::to_tagged_ptr( new Node{std::forward<T>(value)} );
    auto top = m_top_ptr.load(std::memory_order_relaxed);

    while (true) {
        tp::from_tagged_ptr<Node>(new_node)->next_ptr.store( top, std::memory_order_relaxed );
        if (m_top_ptr.compare_exchange_weak( top, tp::increment(new_node), std::memory_order_release, std::memory_order_acquire )) {
            ++m_size;
            return;
        }
    }
}

}  // namespace mtds

#endif //LIBMTDS_TREIBER_STACK_HPP
