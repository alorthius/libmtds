#ifndef MY_QUEUE_QUEUE_HPP
#define MY_QUEUE_QUEUE_HPP

template<typename T>
struct node_t {
    T value;
    node_t* next;
};

template<typename T>
class queue_t {
public:
    queue_t() = default;
    void enqueue(T value);
    T dequeue();

private:
    node_t<T>* head = nullptr;
    node_t<T>* tail = nullptr;
};

template<typename T>
void queue_t<T>::enqueue(T value) {
    auto *temp = new node_t<T>{value, nullptr};
    if (!head) {
        head = tail = temp;
    } else {
        tail->next = temp;
        tail = tail->next;
    }
}

template<typename T>
T queue_t<T>::dequeue() {
    T value = head->value;
    node_t<T>* temp = head;
    head = head->next;
    delete temp;
    return value;
}

#endif //MY_QUEUE_QUEUE_HPP
