// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <gtest/gtest.h>
#include "container_tests.hpp"
#include "mtds/treiber_stack.hpp"

constexpr size_t NUM_PRODUCERS = 4;
constexpr size_t NUM_CONSUMERS = 4;
constexpr size_t NUM_OPERATIONS = 1e5;

class TreiberStackTest : public ::testing::Test {
protected:
    void SetUp() override {
        c1.push(1);
        c2.push(2);
        c2.push(3);
    }

    mtds::TreiberStack<int> c0;
    mtds::TreiberStack<int> c1;
    mtds::TreiberStack<int> c2;
};

TEST_F(TreiberStackTest, IsEmptyInitially) {
    EXPECT_EQ(c0.size(), 0);
    EXPECT_TRUE(c0.empty());

    EXPECT_NE(c1.size(), 0);
    EXPECT_FALSE(c1.empty());
}

TEST_F(TreiberStackTest, TryDequeueWorks) {
    EXPECT_FALSE(c0.try_pop().has_value());

    auto value = c1.try_pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value, 1);
    EXPECT_EQ(c1.size(), 0);

    value = c2.try_pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value, 3);
    EXPECT_EQ(c2.size(), 1);
}

TEST_F(TreiberStackTest, ClearWorks) {
    c2.clear();
    EXPECT_EQ(c2.size(), 0);
    EXPECT_TRUE(c2.empty());
}

TEST_F(TreiberStackTest, EnduranceTest) {
    auto sum = endurance_test(c0, NUM_PRODUCERS + NUM_CONSUMERS, NUM_OPERATIONS);
    EXPECT_EQ(sum, NUM_OPERATIONS);
}

TEST_F(TreiberStackTest, ProducerConsumerTest) {
    auto sum = producer_consumer_test(c0, NUM_PRODUCERS, NUM_CONSUMERS, NUM_OPERATIONS);
    EXPECT_EQ(sum, NUM_OPERATIONS);
}
