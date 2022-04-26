// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MTDSLIB_MPMC_QUEUE_HPP
#define MTDSLIB_MPMC_QUEUE_HPP

#include <atomic>
#include <optional>
#include "mpsc_queue.hpp"
#include "details/free_list.hpp"

namespace mtds {

template<typename T>
class MpmcQueue final : public MpscQueue<T> {
    tp::FreeList<T> m_free_list{};

    void dispose_node(tp::Node<T>* node_ptr) override { m_free_list.push(node_ptr); }
};

}  // namespace mtds

#endif //MTDSLIB_MPMC_QUEUE_HPP
