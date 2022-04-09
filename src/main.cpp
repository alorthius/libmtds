// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <thread>
#include <vector>
#include "mutex_queue.hpp"

int main() {
    mtds::MutexQueue<int> queue;
    std::vector<std::thread> threads;
    for (size_t i = 0; i < 4; ++i) {
        threads.emplace_back(&mtds::MutexQueue<int>::enqueue, &queue, i);
    }
    for (auto& thread: threads) {
        thread.join();
    }
    threads.clear();
    for (size_t i = 0; i < 4; ++i) {
        threads.emplace_back(&mtds::MutexQueue<int>::try_dequeue, &queue);
    }
    for (auto& thread: threads) {
        thread.join();
    }
    threads.clear();
    for (size_t i = 0; i < 4; ++i) {
        threads.emplace_back(&mtds::MutexQueue<int>::dequeue, &queue);
    }
    for (size_t i = 0; i < 4; ++i) {
        threads.emplace_back(&mtds::MutexQueue<int>::enqueue, &queue, i);
    }
    for (auto& thread: threads) {
        thread.join();
    }
    return 0;
}
