// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef LIBMTDS_FREE_LIST_HPP
#define LIBMTDS_FREE_LIST_HPP

#include <cstddef>
#include <atomic>
#include "tagged_ptr.hpp"

namespace mtds::tp {

template<typename T>
class FreeList {
public:
    FreeList() = default;
    ~FreeList() { clear(); };
    FreeList(const FreeList&) = delete;
    FreeList& operator=(const FreeList&) = delete;

    [[nodiscard]] bool empty() const {
        return tp::from_tagged_ptr<Node>(m_top_ptr.load(std::memory_order_relaxed)) == nullptr;
    }

    void clear();
    template<typename U> void push(U&& value);

private:
    using tagged_ptr = tp::tagged_ptr;
    using Node = tp::Node<T>;

    std::atomic<tagged_ptr> m_top_ptr = tp::tagged_nullptr;
};

template<typename T>
template<typename U>
void FreeList<T>::push(U &&value) {
    auto new_node = tp::to_tagged_ptr( new Node{std::forward<T>(value)} );
    auto top = m_top_ptr.load(std::memory_order_relaxed);

    while (true) {
        tp::from_tagged_ptr<Node>(new_node)->next_ptr.store( top, std::memory_order_relaxed );
        if (m_top_ptr.compare_exchange_weak( top, tp::combine_and_increment(new_node, top),
                                             std::memory_order_release, std::memory_order_acquire )) {
            return;
        }
    }
}

template<typename T>
void FreeList<T>::clear() {
    for (auto top = m_top_ptr.load(std::memory_order_relaxed);
         tp::from_tagged_ptr<Node>(top) != nullptr;
         top = tp::from_tagged_ptr<Node>(top)->next_ptr.load(std::memory_order_relaxed)) {
        delete tp::from_tagged_ptr<Node>(top)->value;
        delete tp::from_tagged_ptr<Node>(top);
    }

}

}  // namespace mtds::fl

#endif //LIBMTDS_FREE_LIST_HPP
