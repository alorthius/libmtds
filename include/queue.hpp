// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MY_QUEUE_QUEUE_HPP
#define MY_QUEUE_QUEUE_HPP

#include <mutex>
#include <condition_variable>

template<typename T>
struct Node {
    T m_value;
    Node* m_next_ptr;
    explicit Node(T value);
};

template<typename T>
Node<T>::Node(T value) : m_value{value}, m_next_ptr{nullptr} {}

template<typename T>
class Queue {
public:
    Queue() = default;
    ~Queue();
    void enqueue(const T& value);
    T dequeue();
    bool try_dequeue(T* value_ptr);
    [[nodiscard]] size_t size() const { return m_size; };

private:
    size_t m_size = 0;
    Node<T>* m_head_ptr = nullptr;
    Node<T>* m_tail_ptr = nullptr;
    std::mutex m_mutex;
    std::condition_variable_any m_condition_var;
};

template<typename T>
Queue<T>::~Queue() {
    while (m_head_ptr) {
        Node<T>* temp = m_head_ptr;
        m_head_ptr = m_head_ptr->m_next_ptr;
        delete temp;
    }
}

template<typename T>
void Queue<T>::enqueue(const T& value) {
    m_mutex.lock();
    ++m_size;
    auto* temp = new Node<T>{value};
    if (!m_head_ptr) {
        m_head_ptr = m_tail_ptr = temp;
    } else {
        m_tail_ptr->m_next_ptr = temp;
        m_tail_ptr = m_tail_ptr->m_next_ptr;
    }
    m_condition_var.notify_one();
    m_mutex.unlock();
}

template<typename T>
T Queue<T>::dequeue() {
    m_mutex.lock();
    while (m_size == 0) {
        m_condition_var.wait(m_mutex);
    }
    --m_size;
    T value = m_head_ptr->m_value;
    Node<T>* temp = m_head_ptr;
    m_head_ptr = m_head_ptr->m_next_ptr;
    delete temp;
    m_mutex.unlock();
    return value;
}

template<typename T>
bool Queue<T>::try_dequeue(T* value_ptr) {
    m_mutex.lock();
    if (m_size == 0) {
        m_mutex.unlock();
        return false;
    }
    --m_size;
    new(value_ptr) T(m_head_ptr->m_value);
    Node<T>* temp = m_head_ptr;
    m_head_ptr = m_head_ptr->m_next_ptr;
    delete temp;
    m_mutex.unlock();
    return true;
}

#endif //MY_QUEUE_QUEUE_HPP
