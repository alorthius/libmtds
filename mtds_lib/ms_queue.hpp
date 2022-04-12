// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MTDSLIB_MS_QUEUE_HPP
#define MTDSLIB_MS_QUEUE_HPP

#include <atomic>
#include <optional>

namespace mtds {

template<typename T>
class MsQueue {
public:
    MsQueue();
    ~MsQueue() = default;  // TODO: implement nontrivial destructor
    MsQueue(const MsQueue&) = delete;
    MsQueue& operator=(const MsQueue&) = delete;
    void enqueue(const T& value);
    std::optional<T> dequeue();
    [[nodiscard]] size_t size() const { return m_size; }
private:
    struct TaggedPtr {

        uintptr_t m_ptr : 48;
        unsigned short m_tag = 0;
    };
    struct Node {
        T m_value{};
        TaggedPtr m_next_ptr = TaggedPtr{reinterpret_cast<uintptr_t>(nullptr)};
    };
    std::atomic<size_t> m_size = 0;
    std::atomic<TaggedPtr> m_head_ptr;
    std::atomic<TaggedPtr> m_tail_ptr;
};

template<typename T>
MsQueue<T>::MsQueue() {
    auto temp = reinterpret_cast<uintptr_t>(new Node{});
    m_head_ptr = TaggedPtr{temp};  // dummy node
    m_tail_ptr = TaggedPtr{temp};
}

//template<typename T>
//MsQueue<T>::~MsQueue() {
//    while (m_head_ptr) {
//        auto temp = m_head_ptr;
//        m_head_ptr = m_head_ptr->m_next_ptr;
//        delete temp;
//    }
//}

}  // namespace mtds

#endif //MTDSLIB_MS_QUEUE_HPP
