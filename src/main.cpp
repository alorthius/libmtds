// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <thread>
#include <vector>
#include "queue.hpp"

void try_dequeue_and_delete(Queue<int>& queue) {
    int* value_ptr = static_cast<int*>(::operator new(sizeof(int)));
    queue.try_dequeue(value_ptr);
    delete value_ptr;
}

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
    for (size_t i = 0; i < 4; ++i) {
        threads.emplace_back(try_dequeue_and_delete, std::ref(queue));
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
    threads.clear();
    return 0;
}
