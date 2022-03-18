// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <gtest/gtest.h>
#include <thread>
#include "queue.hpp"

constexpr size_t NUMBER_OF_THREADS = 16;
constexpr size_t NUMBER_OF_OPERATIONS = 10e7;

class QueueTest : public ::testing::Test {
protected:
    void SetUp() override {
        q1.enqueue(1);
        q2.enqueue(2);
        q2.enqueue(3);
    }
    Queue<int> q0;
    Queue<int> q1;
    Queue<int> q2;
};

TEST_F(QueueTest, IsEmptyInitially) {
    EXPECT_EQ(q0.get_size(), 0);
}

TEST_F(QueueTest, TryDequeueWorks) {
    int value;
    EXPECT_FALSE(q0.try_dequeue(&value));

    ASSERT_TRUE(q1.try_dequeue(&value));
    EXPECT_EQ(value, 1);
    EXPECT_EQ(q1.get_size(), 0);

    ASSERT_TRUE(q2.try_dequeue(&value));
    EXPECT_EQ(value, 2);
    EXPECT_EQ(q2.get_size(), 1);
}

void multiple_enqueue(Queue<int>& queue, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        queue.enqueue(static_cast<int>(i));
    }
}

void multiple_dequeue(Queue<int>& queue, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        queue.dequeue();
    }
}

void multiple_enqueue_and_dequeue(Queue<int>& queue, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        if (i % 5 == 4) {
            queue.dequeue();
        } else {
            queue.enqueue(static_cast<int>(i));
        }
    }
}

/*
 * Number of operations = n enqueues = n dequeues
 * 1st phase: n enqueues and 0.25*n dequeues in turn
 * 2nd phase: 0.75*n dequeues
 */
TEST_F(QueueTest, EnduranceTest) {
    size_t operations_per_thread = NUMBER_OF_OPERATIONS / NUMBER_OF_THREADS;
    std::vector<std::thread> threads;
    for (size_t i = 0; i < NUMBER_OF_THREADS; ++i) {
        threads.emplace_back(multiple_enqueue_and_dequeue, std::ref(q0), 5 * operations_per_thread / 4);
    }
    for (auto& thread: threads) {
        thread.join();
    }
    threads.clear();
    for (size_t i = 0; i < NUMBER_OF_THREADS; ++i) {
        threads.emplace_back(multiple_dequeue, std::ref(q0), 3 * operations_per_thread / 4);
    }
    for (auto& thread: threads) {
        thread.join();
    }
}

/*
 * Number of operations = n enqueues = n dequeues
 * Number of threads = n readers + n writers
 */
TEST_F(QueueTest, ProducerConsumerTest) {
    size_t operations_per_thread = NUMBER_OF_OPERATIONS / NUMBER_OF_THREADS;
    std::vector<std::thread> threads;
    for (size_t i = 0; i < NUMBER_OF_THREADS; ++i) {
        if (i % 2 == 0) {
            threads.emplace_back(multiple_enqueue, std::ref(q0), operations_per_thread);
        } else {
            threads.emplace_back(multiple_dequeue, std::ref(q0), operations_per_thread);
        }
    }
    for (auto& thread: threads) {
        thread.join();
    }
}