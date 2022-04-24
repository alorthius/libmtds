// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <gtest/gtest.h>
#include <thread>
#include <random>
#include "details/tagged_ptr.hpp"
#include "details/free_list.hpp"

constexpr size_t NUM_PRODUCERS = 8;
constexpr size_t NUM_OPERATIONS = 10e6;

class FreeListTest : public ::testing::Test {
protected:
    void SetUp() override {
        c1.push(new mtds::tp::Node<int>{});
    }

    mtds::tp::FreeList<mtds::tp::Node<int>*> c0;
    mtds::tp::FreeList<mtds::tp::Node<int>*> c1;
    std::vector<std::thread> threads;
};

TEST_F(FreeListTest, IsEmptyInitially) {
    EXPECT_TRUE(c0.empty());
    EXPECT_FALSE(c1.empty());
}

void multiple_push(mtds::tp::FreeList<mtds::tp::Node<int>*>& container, size_t num_operations) {
    std::random_device r;
    std::default_random_engine e1{r()};
    std::uniform_int_distribution<size_t> uniform_dist{1, 10};

    for (size_t i = 0; i < num_operations; ++i) {
        container.push(new mtds::tp::Node<int>{});
        std::this_thread::sleep_for(std::chrono::microseconds(uniform_dist(e1)));
    }
}

TEST_F(FreeListTest, ClearWorks) {
    auto ops_per_producer = NUM_OPERATIONS / NUM_PRODUCERS;

    for (size_t i = 0; i < NUM_PRODUCERS; ++i) {
        threads.emplace_back(multiple_push, std::ref(c0), ops_per_producer);
    }
    ASSERT_FALSE(c0.empty());

    c0.clear();
    ASSERT_TRUE(c0.empty());
}
