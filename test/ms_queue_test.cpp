// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <gtest/gtest.h>
#include "container_tests.hpp"
#include "ms_queue.hpp"

constexpr size_t NUMBER_OF_THREADS = 8;
constexpr size_t NUMBER_OF_OPERATIONS = 10e6;

class MsQueueTest : public ::testing::Test {
protected:
    void SetUp() override {
        c1.push(1);
        c2.push(2);
        c2.push(3);
    }

    mtds::MsQueue<int> c0;
    mtds::MsQueue<int> c1;
    mtds::MsQueue<int> c2;
};

TEST_F(MsQueueTest, IsEmptyInitially) {
    EXPECT_EQ(c0.size(), 0);
    EXPECT_TRUE(c0.empty());

    EXPECT_NE(c1.size(), 0);
    EXPECT_FALSE(c1.empty());
}

TEST_F(MsQueueTest, TryDequeueWorks) {
    EXPECT_FALSE(c0.try_pop().has_value());

    auto value = c1.try_pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value, 1);
    EXPECT_EQ(c1.size(), 0);

    value = c2.try_pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value, 2);
    EXPECT_EQ(c2.size(), 1);
}

TEST_F(MsQueueTest, ClearWorks) {
    c2.clear();
    EXPECT_EQ(c2.size(), 0);
    EXPECT_TRUE(c2.empty());
}

TEST_F(MsQueueTest, EnduranceTest) {
    auto sum = endurance_test(c0, NUMBER_OF_THREADS, NUMBER_OF_OPERATIONS);
    EXPECT_EQ(sum, NUMBER_OF_OPERATIONS);
}


TEST_F(MsQueueTest, ProducerConsumerTest) {
    auto sum = producer_consumer_test(c0, NUMBER_OF_THREADS, NUMBER_OF_OPERATIONS);
    EXPECT_EQ(sum, NUMBER_OF_OPERATIONS);
}