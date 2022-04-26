#include <benchmark/benchmark.h>
#include "mutex_queue.hpp"
#include "ms_queue.hpp"


template <class Q, class T> void BM_EnqueueDequeue(benchmark::State& state) {
    Q queue;
//    typename Q::value_type v;  //TODO: replace T

    for ([[maybe_unused]] auto _ : state) {
        for (size_t i = state.range(0); i--;) {
            queue.enqueue(T{});
            queue.dequeue();
        }
    }
}

BENCHMARK_TEMPLATE(BM_EnqueueDequeue, mtds::MutexQueue<int>, int)->Arg(10'000'000)->ThreadRange(1, 64)->Unit(benchmark::kSecond)->UseRealTime();
BENCHMARK_TEMPLATE(BM_EnqueueDequeue, mtds::MutexQueue<std::string>, std::string)->Arg(10'000'000)->ThreadRange(1, 64)->Unit(benchmark::kSecond)->UseRealTime();

BENCHMARK_TEMPLATE(BM_EnqueueDequeue, mtds::MsQueue<int>, int)->Arg(1'000'000)->ThreadRange(1, 32)->Unit(benchmark::kSecond)->UseRealTime();
BENCHMARK_TEMPLATE(BM_EnqueueDequeue, mtds::MsQueue<std::string>, std::string)->Arg(1'000'000)->ThreadRange(1, 32)->Unit(benchmark::kSecond)->UseRealTime();

BENCHMARK_MAIN();
