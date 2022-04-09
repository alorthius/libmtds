#ifndef MTDSLIB_MS_QUEUE_HPP
#define MTDSLIB_MS_QUEUE_HPP

#include <atomic>
#include <cstddef>

namespace mtds {

template<typename T>
class MsQueue {
public:
    MsQueue() = default;
    ~MsQueue();
    MsQueue(const MsQueue&) = delete;
    MsQueue& operator=(const MsQueue&) = delete;
    void enqueue(const T& value);
    T dequeue();
    [[nodiscard]] size_t size() const { return m_size; }
private:
    template<typename U>
    struct Node {
        Node() = delete;
        U m_value;
        Node* m_next_ptr = nullptr;
    };
    std::atomic<size_t> m_size = 0;
    Node<T>* m_head_ptr = nullptr;
    Node<T>* m_tail_ptr = nullptr;
};

template<typename T>
MsQueue<T>::~MsQueue() {
    while (m_head_ptr) {
        auto temp = m_head_ptr;
        m_head_ptr = m_head_ptr->m_next_ptr;
        delete temp;
    }
}

}  // namespace mtds

#endif //MTDSLIB_MS_QUEUE_HPP
