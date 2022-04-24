// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <thread>
#include <vector>
#include "mutex_queue.hpp"
#include "ms_queue.hpp"
#include "treiber_stack.hpp"

int main() {
    mtds::TreiberStack<int> c;
    c.push(1);
    std::cout << c.pop() << std::endl;
    return 0;
}
