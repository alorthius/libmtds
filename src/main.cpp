// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include "queue.hpp"

int main(int argc, char *argv[]) {
    queue_t queue;
    queue.enqueue(1);
    queue.enqueue(2);
    std::cout << queue.dequeue() << std::endl;
    std::cout << queue.dequeue() << std::endl;
    queue.enqueue(3);
    queue.enqueue(4);
    std::cout << queue.dequeue() << std::endl;
    std::cout << queue.dequeue() << std::endl;
    return 0;
}
