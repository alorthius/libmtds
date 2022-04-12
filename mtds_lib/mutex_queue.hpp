// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MTDSLIB_MUTEX_QUEUE_HPP
#define MTDSLIB_MUTEX_QUEUE_HPP

#include <mutex>
#include <condition_variable>
#include <optional>
#include <atomic>

namespace mtds {

template<typename T>
class MutexQueue {
public:
    MutexQueue() = default;
    ~MutexQueue();
    MutexQueue(const MutexQueue&) = delete;
    MutexQueue& operator=(const MutexQueue&) = delete;
    void enqueue(const T& value);
    T dequeue();
    std::optional<T> try_dequeue();
    [[nodiscard]] size_t size() const { return m_size; }

private:
    struct Node {
        T m_value{};
        Node* m_next_ptr = nullptr;
    };
    std::atomic<size_t> m_size = 0;
    Node* m_head_ptr = nullptr;
    Node* m_tail_ptr = nullptr;
    std::mutex m_mutex;
    std::condition_variable m_cv_empty;
};

template<typename T>
MutexQueue<T>::~MutexQueue() {
    while (m_head_ptr) {
        auto temp = m_head_ptr;
        m_head_ptr = m_head_ptr->m_next_ptr;
        delete temp;
    }
}

template<typename T>
void MutexQueue<T>::enqueue(const T& value) {
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        ++m_size;
        auto temp = new Node{value};
        if (!m_head_ptr) {
            m_head_ptr = m_tail_ptr = temp;
        } else {
            m_tail_ptr->m_next_ptr = temp;
            m_tail_ptr = m_tail_ptr->m_next_ptr;
        }
    }
    m_cv_empty.notify_one();
}

template<typename T>
T MutexQueue<T>::dequeue() {
    std::unique_lock<std::mutex> lock{m_mutex};
    m_cv_empty.wait(lock, [this]{ return m_size != 0; });
    --m_size;
    auto value = std::move(m_head_ptr->m_value);
    auto temp = m_head_ptr;
    m_head_ptr = m_head_ptr->m_next_ptr;
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
    auto value = std::move(m_head_ptr->m_value);
    auto temp = m_head_ptr;
    m_head_ptr = m_head_ptr->m_next_ptr;
    delete temp;
    return value;
}

}  // namespace mtds

#endif //MTDSLIB_MUTEX_QUEUE_HPP
