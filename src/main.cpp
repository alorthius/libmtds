// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <thread>
#include <vector>
#include "queue.hpp"

int main() {
    Queue<int> queue;
    std::vector<std::thread> threads;
    for (size_t i = 0; i < 4; ++i) {
        threads.emplace_back(&Queue<int>::enqueue, &queue, i);
    }
    for (auto& thread: threads) {
        thread.join();
    }
    threads.clear();
    int values[4];
    for (int& value: values) {
        threads.emplace_back(&Queue<int>::try_dequeue, &queue, &value);
    }
    for (auto& thread: threads) {
        thread.join();
    }
    threads.clear();
    for (size_t i = 0; i < 4; ++i) {
        threads.emplace_back(&Queue<int>::dequeue, &queue);
    }
    for (size_t i = 0; i < 4; ++i) {
        threads.emplace_back(&Queue<int>::enqueue, &queue, i);
    }
    for (auto& thread: threads) {
        thread.join();
    }
    return 0;
}
