// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <gtest/gtest.h>
#include <thread>
#include <random>
#include "details/tagged_ptr.hpp"
#include "details/free_list.hpp"

constexpr size_t NUM_PRODUCERS = 4;
constexpr size_t NUM_OPERATIONS = 10e4;

class FreeListTest : public ::testing::Test {
protected:
    mtds::tp::FreeList<int> c0;
    std::vector<std::thread> threads;
};

void multiple_push(mtds::tp::FreeList<int>& container, size_t num_operations) {
    std::random_device r;
    std::default_random_engine e1{r()};
    std::uniform_int_distribution<size_t> uniform_dist{1, 10};

    for (size_t i = 0; i < num_operations; ++i) {
        container.push(new mtds::tp::Node<int>{});
        std::this_thread::sleep_for(std::chrono::microseconds(uniform_dist(e1)));
    }
}

TEST_F(FreeListTest, PushAndDestructorWork) {
    auto ops_per_producer = NUM_OPERATIONS / NUM_PRODUCERS;

    for (size_t i = 0; i < NUM_PRODUCERS; ++i) {
        threads.emplace_back(multiple_push, std::ref(c0), ops_per_producer);
    }
    for (auto& thread : threads) {
        thread.join();
    }
}
