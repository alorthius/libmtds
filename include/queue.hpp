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
    explicit Node(T value) : m_value{value}, m_next_ptr{nullptr} {};
};

template<typename T>
class Queue {
public:
    Queue() : m_size{0}, m_head_ptr{nullptr}, m_tail_ptr{nullptr} {};
    ~Queue();
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;
    void enqueue(const T& value);
    T dequeue();
    bool try_dequeue(T* value_ptr);
    [[nodiscard]] size_t get_size() const;

private:
    size_t m_size;
    Node<T>* m_head_ptr;
    Node<T>* m_tail_ptr;
    mutable std::mutex m_mutex;
    std::condition_variable m_condition_var;
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
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        ++m_size;
        auto* temp = new Node<T>{value};
        if (!m_head_ptr) {
            m_head_ptr = m_tail_ptr = temp;
        } else {
            m_tail_ptr->m_next_ptr = temp;
            m_tail_ptr = m_tail_ptr->m_next_ptr;
        }
    }
    m_condition_var.notify_one();
}

template<typename T>
T Queue<T>::dequeue() {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_size == 0) {
        m_condition_var.wait(lock);
    }
    --m_size;
    T value = m_head_ptr->m_value;
    Node<T>* temp = m_head_ptr;
    m_head_ptr = m_head_ptr->m_next_ptr;
    delete temp;
    return value;
}

template<typename T>
bool Queue<T>::try_dequeue(T* value_ptr) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_size == 0) {
        return false;
    }
    --m_size;
    new(value_ptr) T(m_head_ptr->m_value);
    Node<T>* temp = m_head_ptr;
    m_head_ptr = m_head_ptr->m_next_ptr;
    delete temp;
    return true;
}

template<typename T>
size_t Queue<T>::get_size() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_size;
}

#endif //MY_QUEUE_QUEUE_HPP
