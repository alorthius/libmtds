// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include "queue.hpp"

int main() {
    queue_t<int> queue_int;
    queue_int.enqueue(1);
    queue_int.enqueue(2);
    std::cout << queue_int.dequeue() << std::endl;
    std::cout << queue_int.dequeue() << std::endl;
    queue_int.enqueue(3);
    queue_int.enqueue(4);
    std::cout << queue_int.dequeue() << std::endl;
    std::cout << queue_int.dequeue() << std::endl;

    queue_t<std::string> queue_string;
    queue_string.enqueue("one");
    queue_string.enqueue("two");
    std::cout << queue_string.dequeue() << std::endl;
    std::cout << queue_string.dequeue() << std::endl;
    queue_string.enqueue("three");
    queue_string.enqueue("four");
    std::cout << queue_string.dequeue() << std::endl;
    std::cout << queue_string.dequeue() << std::endl;
    return 0;
}
