// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <benchmark/benchmark.h>
#include "mtds/two_mutex_queue.hpp"
#include "mtds/ms_queue.hpp"
#include "mtds/details/backoff.hpp"

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

template <typename Q, typename T = typename Q::value_type> void BM_EnqueueOnceDequeue(benchmark::State& state) {
    static Q queue;

    if (state.thread_index() == 0) {
        queue.clear();
    }

    if (state.thread_index() == 0) {
        for ([[maybe_unused]] auto _ : state) {
            for (size_t i = (state.range(0)) * (state.threads() - 1); i--;) {
                queue.enqueue( T{} );
            }
        }
    } else {
        for ([[maybe_unused]] auto _ : state) {
            for (size_t i = state.range(0); i--;) {
                queue.dequeue();
            }
        }
    }

    if (state.thread_index() == 0) {
        queue.clear();
    }
}

#define TEST_ON_TYPE(type) \
    BENCHMARK(BM_EnqueueDequeue< mtds::TwoMutexQueue<type> >)->Arg(1'000'000)->ThreadRange(1, 16)->Unit(benchmark::kMillisecond)->UseRealTime();\
    BENCHMARK(BM_EnqueueDequeue< mtds::MsQueue<type> >)->Arg(1'000'000)->ThreadRange(1, 16)->Unit(benchmark::kMillisecond)->UseRealTime();\
    BENCHMARK(BM_EnqueueDequeue< mtds::MsQueue<type, mtds::backoff::yield_backoff> >)->Arg(1'000'000)->ThreadRange(1, 16)->Unit(benchmark::kMillisecond)->UseRealTime();\
    BENCHMARK(BM_EnqueueDequeueOnce< mtds::TwoMutexQueue<type> >)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();\
    BENCHMARK(BM_EnqueueDequeueOnce< mtds::MsQueue<type> >)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();\
    BENCHMARK(BM_EnqueueDequeueOnce< mtds::MsQueue<type, mtds::backoff::yield_backoff> >)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();\
    BENCHMARK(BM_EnqueueOnceDequeue< mtds::TwoMutexQueue<type> >)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();\
    BENCHMARK(BM_EnqueueOnceDequeue< mtds::MsQueue<type> >)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();\
    BENCHMARK(BM_EnqueueOnceDequeue< mtds::MsQueue<type, mtds::backoff::yield_backoff> >)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();

TEST_ON_TYPE(int);
TEST_ON_TYPE(std::string);
TEST_ON_TYPE(MyPair);

BENCHMARK_MAIN();
