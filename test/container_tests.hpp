// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef LIBMTDS_CONTAINER_TESTS_HPP
#define LIBMTDS_CONTAINER_TESTS_HPP

#include <random>
#include <thread>
#include <numeric>

template<typename C>
void multiple_push(C& container, size_t num_operations) {
    std::random_device r;
    std::default_random_engine e1{r()};
    std::uniform_int_distribution<size_t> uniform_dist{1, 10};

    for (size_t i = 0; i < num_operations; ++i) {
        container.push(1);
        std::this_thread::sleep_for(std::chrono::microseconds(uniform_dist(e1)));
    }
}

template<typename C>
void multiple_pop(C& container, size_t num_operations, int& sum) {
    std::random_device r;
    std::default_random_engine e1{r()};
    std::uniform_int_distribution<size_t> uniform_dist{1, 5};

    for (size_t i = 0; i < num_operations; ++i) {
        sum += container.pop();
        std::this_thread::sleep_for(std::chrono::microseconds(uniform_dist(e1)));
    }
}

template<typename C>
void multiple_push_and_pop(C& container, size_t num_operations, int& sum) {
    std::random_device r;
    std::default_random_engine e1{r()};
    std::uniform_int_distribution<size_t> uniform_dist_1{1, 10};
    std::uniform_int_distribution<size_t> uniform_dist_2{1, 5};

    for (size_t i = 0; i < num_operations; ++i) {
        if (i % 5 != 4) {
            container.push(1);
            std::this_thread::sleep_for(std::chrono::microseconds(uniform_dist_1(e1)));
        } else {
            sum += container.pop();
            std::this_thread::sleep_for(std::chrono::microseconds(uniform_dist_2(e1)));
        }
    }
}

/*
 * Number of operations = n pushes = n pops
 * 1st phase: n pushes and 0.25*n pops in turn
 * 2nd phase: 0.75*n pops
 */
template<typename C>
int endurance_test(C& container, size_t num_threads, size_t num_operations) {
    size_t operations_per_thread = num_operations / num_threads;
    std::vector<std::thread> threads;
    std::vector<int> sums(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back(multiple_push_and_pop<C>, std::ref(container),
                             5 * operations_per_thread / 4, std::ref(sums[i]));
    }
    for (auto& thread: threads) {
        thread.join();
    }
    threads.clear();

    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back(multiple_pop<C>, std::ref(container), 3 * operations_per_thread / 4,
                             std::ref(sums[i]));
    }
    for (auto& thread: threads) {
        thread.join();
    }
    return std::accumulate(sums.begin(), sums.end(), 0);
}

/*
 * Number of operations = n pushes = n pops
 * Number of threads = n readers + m writers
 */
template<typename C>
int producer_consumer_test(C& container, size_t num_producers,
                           size_t num_consumers, size_t num_operations) {
    size_t ops_per_producer = num_operations / num_producers;
    size_t ops_per_consumer = num_operations / num_consumers;
    std::vector<std::thread> threads;
    std::vector<int> sums(num_consumers);

    for (size_t i = 0; i < num_producers; ++i) {
        threads.emplace_back(multiple_push<C>, std::ref(container), ops_per_producer);
    }
    for (size_t i = 0; i < num_consumers; ++i) {
        threads.emplace_back(multiple_pop<C>, std::ref(container), ops_per_consumer,
                             std::ref(sums[i]));
    }
    for (auto& thread: threads) {
        thread.join();
    }
    return std::accumulate(sums.begin(), sums.end(), 0);
}

#endif //LIBMTDS_CONTAINER_TESTS_HPP
