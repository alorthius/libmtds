#ifndef MY_QUEUE_QUEUE_HPP
#define MY_QUEUE_QUEUE_HPP

struct node_t {
    int value;
    node_t* next;
};

class queue_t {
public:
    queue_t() = default;
    void enqueue(int value);
    int dequeue();

private:
    node_t* head = nullptr;
    node_t* tail = nullptr;
};

#endif //MY_QUEUE_QUEUE_HPP
