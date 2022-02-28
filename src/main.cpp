// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <thread>
#include <vector>
#include "queue.hpp"


void print_enqueue(Queue<int>& queue, int i, std::mutex& mutex) {
    mutex.lock();
    queue.enqueue(i);
    std::cout << i << " ";
    mutex.unlock();
}

void print_dequeue(Queue<int>& queue, std::mutex& mutex) {
    mutex.lock();
    int* value_ptr = queue.dequeue();
    std::cout << *value_ptr << " ";
    delete value_ptr;
    mutex.unlock();
}

int main() {
    std::mutex mutex;
    Queue<int> queue;
    std::vector<std::thread> threads;
    std::cout << "Enqueue: ";
    for (size_t i = 0; i < 4; ++i) {
        threads.emplace_back(print_enqueue, std::ref(queue), i, std::ref(mutex));
    }
    for (auto& thread: threads) {
        thread.join();
    }
    threads.clear();
    std::cout << std::endl << "Dequeue: ";
    for (size_t i = 0; i < 4; ++i) {
        threads.emplace_back(print_dequeue, std::ref(queue), std::ref(mutex));
    }
    for (auto& thread: threads) {
        thread.join();
    }
    std::cout << std::endl;
    queue.dequeue();
    queue.enqueue(1);
    queue.enqueue(2);
    return 0;
}
