// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MTDS_MPMC_STACK_HPP
#define MTDS_MPMC_STACK_HPP

#include "mpsc_stack.hpp"
#include "details/free_list.hpp"

namespace mtds {

template<typename T>
class MpmcStack final : public MpscStack<T> {
    tp::FreeList<T> m_free_list{};

    void dispose_node(tp::Node<T>* node_ptr) override { m_free_list.push(node_ptr); }
};

}  // namespace mtds

#endif //MTDS_MPMC_STACK_HPP
