// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef LIBMTDS_CONTAINER_TESTS_HPP
#define LIBMTDS_CONTAINER_TESTS_HPP

template<typename C>
void multiple_push(C& container, size_t num_operations);

template<typename C>
void multiple_pop(C& container, size_t num_operations, int& sum);

template<typename C>
void multiple_push_and_pop(C& container, size_t num_operations, int& sum);

/*
 * Number of operations = n pushes = n pops
 * 1st phase: n pushes and 0.25*n pops in turn
 * 2nd phase: 0.75*n pops
 */
template<typename C>
int endurance_test(C& container, size_t num_threads, size_t num_operations);

/*
 * Number of operations = n pushes = n pops
 * Number of threads = n readers + n writers
 */
template<typename C>
int producer_consumer_test(C& container, size_t num_threads, size_t num_operations);

#endif //LIBMTDS_CONTAINER_TESTS_HPP
