// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MTDS_TWO_MUTEX_QUEUE_HPP
#define MTDS_TWO_MUTEX_QUEUE_HPP

#include <mutex>
#include <condition_variable>
#include <optional>
#include <atomic>

namespace mtds {

template<typename T>
class TwoMutexQueue {
public:
    using value_type = T;
    using size_type = size_t;

    TwoMutexQueue();
    ~TwoMutexQueue();
    TwoMutexQueue(const TwoMutexQueue&) = delete;
    TwoMutexQueue& operator=(const TwoMutexQueue&) = delete;

    [[nodiscard]] bool empty();

    void clear() { while (try_dequeue().has_value()) continue; }
    template<typename U> void enqueue(U&& value);
    std::optional<T> try_dequeue();
    T dequeue();

    // Synonyms
    template<typename U> void push(U&& value) { enqueue(std::forward<U>(value)); }
    std::optional<T> try_pop() { return try_dequeue(); }
    T pop() { return dequeue(); }

private:
    struct Node {
        T value{};
        std::atomic<Node*> next_ptr = nullptr;
    };
    Node* m_head_ptr = nullptr;
    Node* m_tail_ptr = nullptr;
    std::mutex m_head_mutex{};
    std::mutex m_tail_mutex{};
    std::condition_variable m_cv_empty{};
};

template<typename T>
TwoMutexQueue<T>::TwoMutexQueue() {
    auto dummy = new Node{};
    m_head_ptr = m_tail_ptr = dummy;
}

template<typename T>
TwoMutexQueue<T>::~TwoMutexQueue() {
    while (auto temp = m_head_ptr) {
        m_head_ptr = temp->next_ptr;
        delete temp;
    }
}

template<typename T>
[[nodiscard]] bool TwoMutexQueue<T>::empty() {
    std::lock_guard<std::mutex> lock{m_head_mutex};
    return m_head_ptr->next_ptr.load() == nullptr;
}

template<typename T>
template<typename U>
void TwoMutexQueue<T>::enqueue(U&& value) {
    auto temp = new Node{std::forward<U>(value)};
    {
        std::lock_guard<std::mutex> lock{m_tail_mutex};
        m_tail_ptr->next_ptr.store(temp);
        m_tail_ptr = temp;
    }
    m_cv_empty.notify_one();
}

template<typename T>
T TwoMutexQueue<T>::dequeue() {
    T value;
    {
        std::unique_lock<std::mutex> lock{m_head_mutex};
        auto node = m_head_ptr;
        auto new_head = m_head_ptr->next_ptr.load();
        while (new_head == nullptr) {
            m_cv_empty.wait(lock);
            node = m_head_ptr;
            new_head = m_head_ptr->next_ptr.load();
        }
        value = std::move(new_head->value);
        m_head_ptr = new_head;
        delete node;
    }
    return value;
}

template<typename T>
std::optional<T> TwoMutexQueue<T>::try_dequeue() {
    T value;
    {
        std::unique_lock<std::mutex> lock{m_head_mutex};
        auto node = m_head_ptr;
        auto new_head = m_head_ptr->next_ptr.load();
        while (new_head == nullptr) {
            return {};
        }
        value = std::move(new_head->value);
        m_head_ptr = new_head;
        delete node;
    }
    return value;
}

}

#endif  // MTDS_TWO_MUTEX_QUEUE_HPP