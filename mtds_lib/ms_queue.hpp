// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

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
    struct TaggedPtr {
        TaggedPtr() = delete;

        uintptr_t m_ptr : 48;
        unsigned short m_tag = 0;
    };
    struct Node {
        T m_value{};
        TaggedPtr m_next_ptr = nullptr;
    };
    std::atomic<size_t> m_size = 0;
    std::atomic<TaggedPtr> m_head_ptr = TaggedPtr{new Node{}};  // dummy node
    std::atomic<TaggedPtr> m_tail_ptr = m_head_ptr;
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
