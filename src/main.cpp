// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <thread>
#include <vector>
#include "queue.hpp"

void print_enqueue(queue_t<int>& queue, int i) {
    queue.enqueue(i);
    std::cout << i << " ";
}

void print_dequeue(queue_t<int>& queue) {
    std::cout << queue.dequeue() << " ";
}

int main() {
    queue_t<int> queue;
    std::vector<std::thread> threads;
    std::cout << "Enqueue: ";
    for (size_t i = 0; i < 4; ++i) {
        threads.emplace_back(print_enqueue, std::ref(queue), i);
    }
    for (auto& thread: threads) {
        thread.join();
    }
    threads.clear();
    std::cout << std::endl << "Dequeue: ";
    for (size_t i = 0; i < 4; ++i) {
        threads.emplace_back(print_dequeue, std::ref(queue));
    }
    for (auto& thread: threads) {
        thread.join();
    }
    std::cout << std::endl;
    return 0;
}
