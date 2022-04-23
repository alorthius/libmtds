#include <benchmark/benchmark.h>
#include "mutex_queue.hpp"

static void BM_Enqueue(benchmark::State& state) {
    mtds::MutexQueue<std::string> queue;

    for ([[maybe_unused]] auto _ : state) {
        queue.enqueue("123");
    }
}

BENCHMARK(BM_Enqueue)->Arg(100);

BENCHMARK_MAIN();
