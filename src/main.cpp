// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "mpsc_queue.hpp"
#include "mpmc_queue.hpp"
#include "mpsc_stack.hpp"
#include "mpmc_stack.hpp"

int main() {
    mtds::MpscQueue<int> c1;
    mtds::MpmcQueue<int> c2;
    mtds::MpscStack<int> c3;
    mtds::MpmcStack<int> c4;
    return 0;
}
