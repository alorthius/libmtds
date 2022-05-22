// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <benchmark/benchmark.h>
#include "mtds/two_mutex_queue.hpp"
#include "mtds/mutex_queue.hpp"
#include "mtds/mpmc_queue.hpp"
#include "mtds/mpsc_queue.hpp"

struct MyPair {
    int first;
    int second;
    [[nodiscard]] int sum() const { return first + second; }
};

template <typename Q, typename T = typename Q::value_type> void BM_EnqueueDequeue(benchmark::State& state) {
    static Q queue;

    if (state.thread_index() == 0) {
        queue.clear();
    }

    for ([[maybe_unused]] auto _ : state) {
        for (size_t i = state.range(0); i--;) {
            if (i % 3 == 2) {
                queue.dequeue();
            } else {
                queue.enqueue( T{} );
            }
        }
        for (size_t i = state.range(0); i--;) {
            if (i % 3 == 0) {
                queue.enqueue( T{} );
            } else {
                queue.dequeue();
            }
        }
        for (size_t i = state.range(0); i--;) {
            if (i % 2 == 0) {
                queue.enqueue( T{} );
            } else {
                queue.dequeue();
            }
        }
    }

    if (state.thread_index() == 0) {
        queue.clear();
    }
}

template <typename Q, typename T = typename Q::value_type> void BM_EnqueueDequeueOnce(benchmark::State& state) {
    static Q queue;

    if (state.thread_index() == 0) {
        queue.clear();
    }

    if (state.thread_index() == 0) {
        for ([[maybe_unused]] auto _ : state) {
            for (size_t i = (state.range(0)) * (state.threads() - 1); i--;) {
                queue.dequeue();
            }
        }
    } else {
        for ([[maybe_unused]] auto _ : state) {
            for (size_t i = state.range(0); i--;) {
                queue.enqueue( T{} );
            }
        }
    }

    if (state.thread_index() == 0) {
        queue.clear();
    }
}

#define TEST_ON_TYPE(type) \
    BENCHMARK(BM_EnqueueDequeue< mtds::TwoMutexQueue<type> >)->Arg(1'000'000)->ThreadRange(1, 16)->Unit(benchmark::kMillisecond)->UseRealTime();\
    BENCHMARK(BM_EnqueueDequeue< mtds::MutexQueue<type> >)->Arg(1'000'000)->ThreadRange(1, 16)->Unit(benchmark::kMillisecond)->UseRealTime();\
    BENCHMARK(BM_EnqueueDequeue< mtds::MpscQueue <type> >)->Arg(1'000'000)->Threads(1)->Unit(benchmark::kMillisecond)->UseRealTime();\
    BENCHMARK(BM_EnqueueDequeue< mtds::MpmcQueue <type> >)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();\
    BENCHMARK(BM_EnqueueDequeueOnce< mtds::TwoMutexQueue<type> >)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();\
    BENCHMARK(BM_EnqueueDequeueOnce< mtds::MutexQueue<type> >)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();\
    BENCHMARK(BM_EnqueueDequeueOnce< mtds::MpscQueue <type> >)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();\
    BENCHMARK(BM_EnqueueDequeueOnce< mtds::MpmcQueue <type> >)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();

TEST_ON_TYPE(int);
TEST_ON_TYPE(std::string);
TEST_ON_TYPE(MyPair);

// BENCHMARK(BM_EnqueueDequeueOnce< mtds::MpmcQueue <int> >)->Arg(1'000'000)->Threads(16)->Unit(benchmark::kMillisecond)->UseRealTime();

BENCHMARK_MAIN();
