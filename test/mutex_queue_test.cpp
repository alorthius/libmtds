// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <gtest/gtest.h>
#include <thread>
#include <numeric>
#include <random>
#include "mutex_queue.hpp"

constexpr size_t NUMBER_OF_THREADS = 8;
constexpr size_t NUMBER_OF_OPERATIONS = 10e6;

class QueueTest : public ::testing::Test {
protected:
    void SetUp() override {
        q1.enqueue(1);
        q2.enqueue(2);
        q2.enqueue(3);
    }
    mtds::MutexQueue<int> q0;
    mtds::MutexQueue<int> q1;
    mtds::MutexQueue<int> q2;
};

TEST_F(QueueTest, IsEmptyInitially) {
    EXPECT_EQ(q0.size(), 0);
}

TEST_F(QueueTest, TryDequeueWorks) {
    EXPECT_FALSE(q0.try_dequeue().has_value());

    auto value = q1.try_dequeue();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value, 1);
    EXPECT_EQ(q1.size(), 0);

    value = q2.try_dequeue();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value, 2);
    EXPECT_EQ(q2.size(), 1);
}

void multiple_enqueue(mtds::MutexQueue<int>& queue, size_t number_of_operations) {
    std::random_device r;
    std::default_random_engine e1{r()};
    std::uniform_int_distribution<size_t> uniform_dist{1, 10};
    for (size_t i = 0; i < number_of_operations; ++i) {
        queue.enqueue(1);
        std::this_thread::sleep_for(std::chrono::microseconds(uniform_dist(e1)));
    }
}

void multiple_dequeue(mtds::MutexQueue<int>& queue, size_t number_of_operations, int& sum) {
    std::random_device r;
    std::default_random_engine e1{r()};
    std::uniform_int_distribution<size_t> uniform_dist{1, 5};
    for (size_t i = 0; i < number_of_operations; ++i) {
        sum += queue.dequeue();
        std::this_thread::sleep_for(std::chrono::microseconds(uniform_dist(e1)));
    }
}

void multiple_enqueue_and_dequeue(mtds::MutexQueue<int>& queue, size_t number_of_operations, int& sum) {
    std::random_device r;
    std::default_random_engine e1{r()};
    std::uniform_int_distribution<size_t> uniform_dist_1{1, 10};
    std::uniform_int_distribution<size_t> uniform_dist_2{1, 5};
    for (size_t i = 0; i < number_of_operations; ++i) {
        if (i % 5 != 4) {
            queue.enqueue(1);
            std::this_thread::sleep_for(std::chrono::microseconds(uniform_dist_1(e1)));
        } else {
            sum += queue.dequeue();
            std::this_thread::sleep_for(std::chrono::microseconds(uniform_dist_2(e1)));
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
    std::vector<int> sums(NUMBER_OF_THREADS);
    for (size_t i = 0; i < NUMBER_OF_THREADS; ++i) {
        threads.emplace_back(multiple_enqueue_and_dequeue, std::ref(q0),
                             5 * operations_per_thread / 4, std::ref(sums[i]));
    }
    for (auto& thread: threads) {
        thread.join();
    }
    threads.clear();
    for (size_t i = 0; i < NUMBER_OF_THREADS; ++i) {
        threads.emplace_back(multiple_dequeue, std::ref(q0), 3 * operations_per_thread / 4,
                             std::ref(sums[i]));
    }
    for (auto& thread: threads) {
        thread.join();
    }
    EXPECT_EQ(std::accumulate(sums.begin(), sums.end(), 0), NUMBER_OF_OPERATIONS);
}

/*
 * Number of operations = n enqueues = n dequeues
 * Number of threads = n readers + n writers
 */
TEST_F(QueueTest, ProducerConsumerTest) {
    size_t operations_per_thread = 2 * NUMBER_OF_OPERATIONS / NUMBER_OF_THREADS;
    std::vector<std::thread> threads;
    std::vector<int> sums(NUMBER_OF_THREADS);
    for (size_t i = 0; i < NUMBER_OF_THREADS; ++i) {
        if (i % 2 == 0) {
            threads.emplace_back(multiple_enqueue, std::ref(q0), operations_per_thread);
        } else {
            threads.emplace_back(multiple_dequeue, std::ref(q0), operations_per_thread,
                                 std::ref(sums[i]));
        }
    }
    for (auto& thread: threads) {
        thread.join();
    }
    EXPECT_EQ(std::accumulate(sums.begin(), sums.end(), 0), NUMBER_OF_OPERATIONS);
}
