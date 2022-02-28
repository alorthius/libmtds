#ifndef MY_QUEUE_QUEUE_HPP
#define MY_QUEUE_QUEUE_HPP

#include <mutex>

template<typename T>
struct Node {
    T value;
    Node* next;
};

template<typename T>
class Queue {
public:
    Queue() = default;
    ~Queue();
    void enqueue(const T& value);
    T* dequeue();
    [[nodiscard]] size_t size() const { return size_m; };

private:
    std::mutex mutex;
    Node<T>* head = nullptr;
    Node<T>* tail = nullptr;
    size_t size_m = 0;
};

template<typename T>
Queue<T>::~Queue() {
    while (head) {
        Node<T>* temp = head;
        head = head->next;
        delete temp;
    }
}

template<typename T>
void Queue<T>::enqueue(const T& value) {
    mutex.lock();
    ++size_m;
    auto* temp = new Node<T>{value, nullptr};
    if (!head) {
        head = tail = temp;
    } else {
        tail->next = temp;
        tail = tail->next;
    }
    mutex.unlock();
}

template<typename T>
T* Queue<T>::dequeue() {
    mutex.lock();
    if (size_m == 0) {
        mutex.unlock();
        return nullptr;
    }
    --size_m;
    auto* value_ptr = new T(head->value);
    Node<T>* temp = head;
    head = head->next;
    delete temp;
    mutex.unlock();
    return value_ptr;
}

#endif //MY_QUEUE_QUEUE_HPP
