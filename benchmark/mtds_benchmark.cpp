#include <benchmark/benchmark.h>
#include "mutex_queue.hpp"
#include "mpmc_queue.hpp"
#include "mpsc_queue.hpp"

struct MyPair {
    int first;
    int second;
    [[nodiscard]] int sum() const { return first + second; }
};


template <class Q> void BM_EnqueueDequeue(benchmark::State& state) {
    Q queue;
    typename Q::value_type v;

    for ([[maybe_unused]] auto _ : state) {
        for (size_t i = state.range(0); i--;) {
            queue.enqueue( v );
        }
        for (size_t i = state.range(0); i--;) {
            queue.dequeue();
        }
    }
}

// Test on int
BENCHMARK_TEMPLATE(BM_EnqueueDequeue, mtds::MutexQueue<int>)->Arg(10'000'000)->ThreadRange(1, 16)->Unit(benchmark::kMillisecond)->UseRealTime();
BENCHMARK_TEMPLATE(BM_EnqueueDequeue, mtds::MpscQueue<int>)->Arg(10'000'000)->Threads(1)->Unit(benchmark::kMillisecond)->UseRealTime();
BENCHMARK_TEMPLATE(BM_EnqueueDequeue, mtds::MpmcQueue<int>)->Arg(10'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();

// Test on std::string
BENCHMARK_TEMPLATE(BM_EnqueueDequeue, mtds::MutexQueue<std::string>)->Arg(10'000'000)->ThreadRange(1, 16)->Unit(benchmark::kMillisecond)->UseRealTime();
BENCHMARK_TEMPLATE(BM_EnqueueDequeue, mtds::MpscQueue<std::string>)->Arg(10'000'000)->Threads(1)->Unit(benchmark::kMillisecond)->UseRealTime();
BENCHMARK_TEMPLATE(BM_EnqueueDequeue, mtds::MpmcQueue<std::string>)->Arg(10'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();

// Test on MyPair struct
BENCHMARK_TEMPLATE(BM_EnqueueDequeue, mtds::MutexQueue<MyPair>)->Arg(10'000'000)->ThreadRange(1, 16)->Unit(benchmark::kMillisecond)->UseRealTime();
BENCHMARK_TEMPLATE(BM_EnqueueDequeue, mtds::MpscQueue<MyPair>)->Arg(10'000'000)->Threads(1)->Unit(benchmark::kMillisecond)->UseRealTime();
BENCHMARK_TEMPLATE(BM_EnqueueDequeue, mtds::MpmcQueue<MyPair>)->Arg(10'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();

BENCHMARK_MAIN();
