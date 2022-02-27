#include "queue.hpp"

void queue_t::enqueue(int value) {
    if (!head) {
        head = tail = new node_t{value, nullptr};
    } else {
        auto *temp = new node_t{value, nullptr};
        tail->next = temp;
        tail = tail->next;
    }
}

int queue_t::dequeue() {
    int value = head->value;
    node_t* temp = head;
    head = head->next;
    delete temp;
    return value;
}