// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MTDS_MUTEX_QUEUE_HPP
#define MTDS_MUTEX_QUEUE_HPP

#include <mutex>
#include <condition_variable>
#include <optional>
#include <atomic>

namespace mtds {

template<typename T>
class MutexQueue {
public:
    using value_type = T;
    using size_type = size_t;

    MutexQueue() = default;
    ~MutexQueue();
    MutexQueue(const MutexQueue&) = delete;
    MutexQueue& operator=(const MutexQueue&) = delete;

    [[nodiscard]] bool empty() const { return m_size == 0; }
    [[nodiscard]] size_type size() const { return m_size; }

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
        Node* next_ptr = nullptr;
    };
    std::atomic<size_type> m_size = 0;
    Node* m_head_ptr = nullptr;
    Node* m_tail_ptr = nullptr;
    std::mutex m_mutex;
    std::condition_variable m_cv_empty;
};

template<typename T>
MutexQueue<T>::~MutexQueue() {
    while (m_head_ptr) {
        auto temp = m_head_ptr;
        m_head_ptr = m_head_ptr->next_ptr;
        delete temp;
    }
}

template<typename T>
template<typename U>
void MutexQueue<T>::enqueue(U&& value) {
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        ++m_size;
        auto temp = new Node{std::forward<U>(value)};
        if (!m_head_ptr) {
            m_head_ptr = m_tail_ptr = temp;
        } else {
            m_tail_ptr->next_ptr = temp;
            m_tail_ptr = m_tail_ptr->next_ptr;
        }
    }
    m_cv_empty.notify_one();
}

template<typename T>
T MutexQueue<T>::dequeue() {
    std::unique_lock<std::mutex> lock{m_mutex};
    m_cv_empty.wait(lock, [this]{ return m_size != 0; });
    --m_size;
    auto value = std::move(m_head_ptr->value);
    auto temp = m_head_ptr;
    m_head_ptr = m_head_ptr->next_ptr;
    delete temp;
    return value;
}

template<typename T>
std::optional<T> MutexQueue<T>::try_dequeue() {
    std::lock_guard<std::mutex> lock{m_mutex};
    if (m_size == 0) {
        return {};
    }
    --m_size;
    auto value = std::move(m_head_ptr->value);
    auto temp = m_head_ptr;
    m_head_ptr = m_head_ptr->next_ptr;
    delete temp;
    return value;
}

}  // namespace mtds

#endif //MTDS_MUTEX_QUEUE_HPP
