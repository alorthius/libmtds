#include <gtest/gtest.h>
#include <thread>
#include "queue.hpp"

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
    EXPECT_EQ(q0.size(), 0);
}

TEST_F(QueueTest, TryDequeueWorks) {
    int* value_ptr = static_cast<int*>(::operator new(sizeof(int)));
    EXPECT_FALSE(q0.try_dequeue(value_ptr));

    ASSERT_TRUE(q1.try_dequeue(value_ptr));
    EXPECT_EQ(*value_ptr, 1);
    EXPECT_EQ(q1.size(), 0);

    ASSERT_TRUE(q2.try_dequeue(value_ptr));
    EXPECT_EQ(*value_ptr, 2);
    EXPECT_EQ(q2.size(), 1);
    delete value_ptr;
}

void enqueue_test(Queue<int>& queue) {
    for (size_t i = 0; i < 1000; ++i) {
        queue.enqueue(i);
    }
}

void try_dequeue_test(Queue<int>& queue) {
    for (size_t i = 0; i < 1000; ++i) {
        int* value_ptr = nullptr;
        ASSERT_EQ(queue.try_dequeue(value_ptr), true);
        ASSERT_NE(value_ptr, nullptr);
        delete value_ptr;
    }
}

void dequeue_test(Queue<int>& queue) {
    for (size_t i = 0; i < 1000; ++i) {
        if (i % 2 == 0) {
            queue.dequeue();
        } else {
            queue.enqueue(i);
        }
    }
}

TEST_F(QueueTest, QueueWorksUnderMultipleThreads) {
    for (size_t i = 0; i < 10e12; ++i) {
        std::vector<std::thread> threads;
        for (size_t j = 0; j < 4; ++j) {
            threads.emplace_back(enqueue_test, std::ref(q0));
        }
        threads.clear();
        ASSERT_EQ(q0.size(), 4000);
        for (size_t j = 0; j < 4; ++j) {
            threads.emplace_back(try_dequeue_test, std::ref(q0));
        }
        threads.clear();
        ASSERT_EQ(q0.size(), 0);
        for (size_t j = 0; j < 4; ++j) {
            threads.emplace_back(dequeue_test, std::ref(q0));
        }
    }
}