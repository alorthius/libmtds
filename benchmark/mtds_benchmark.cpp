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

template<typename T, template<typename> typename Q>
class QueueFixture : public benchmark::Fixture {
public:
    Q<T>* queue = nullptr;

    void SetUp(const ::benchmark::State& state) override {
        if (state.thread_index() == 0) {
            queue = new Q<T>;
        }
    }

    void TearDown(const ::benchmark::State& state) override {
        if (state.thread_index() == 0) {
            delete queue;
        }
    }

    void EnqueueDequeue(benchmark::State& state);

    void EnqueueDequeueOnce(benchmark::State& state);

    void EnqueueOnceDequeue(benchmark::State& state);
};

template<typename T, template<typename> typename Q>
void QueueFixture<T, Q>::EnqueueDequeue(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        for (size_t i = state.range(0); i--;) {
            if (i % 3 == 2) {
                queue->dequeue();
            } else {
                queue->enqueue( T{} );
            }
        }
        for (size_t i = state.range(0); i--;) {
            if (i % 3 == 0) {
                queue->enqueue( T{} );
            } else {
                queue->dequeue();
            }
        }
        for (size_t i = state.range(0); i--;) {
            if (i % 2 == 0) {
                queue->enqueue( T{} );
            } else {
                queue->dequeue();
            }
        }
    }
}

template<typename T, template<typename> typename Q>
void QueueFixture<T, Q>::EnqueueDequeueOnce(benchmark::State& state) {
    if (state.thread_index() == 0) {
        for ([[maybe_unused]] auto _ : state) {
            for (size_t i = (state.range(0)) * (state.threads() - 1); i--;) {
                queue->dequeue();
            }
        }
    } else {
        for ([[maybe_unused]] auto _ : state) {
            for (size_t i = state.range(0); i--;) {
                queue->enqueue( T{} );
            }
        }
    }
}

template<typename T, template<typename> typename Q>
void QueueFixture<T, Q>::EnqueueOnceDequeue(benchmark::State& state) {
    if (state.thread_index() == 0) {
        for ([[maybe_unused]] auto _ : state) {
            for (size_t i = (state.range(0)) * (state.threads() - 1); i--;) {
                queue->enqueue( T{} );
            }
        }
    } else {
        for ([[maybe_unused]] auto _ : state) {
            for (size_t i = state.range(0); i--;) {
                queue->dequeue();
            }
        }
    }
}

// I am truly sorry for this mess, but my attempt to write a macro failed miserably

BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueDequeueTwoMutexQueueInt, int, mtds::TwoMutexQueue)(benchmark::State& state) { QueueFixture::EnqueueDequeue(state); }
BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueDequeueMsQueueInt, int, mtds::MsQueue)(benchmark::State& state) { EnqueueDequeue(state); }

BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueDequeueOnceTwoMutexQueueInt, int, mtds::TwoMutexQueue)(benchmark::State& state) { EnqueueDequeueOnce(state); }
BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueDequeueOnceMsQueueInt, int, mtds::MsQueue)(benchmark::State& state) { EnqueueDequeueOnce(state); }

BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueOnceDequeueTwoMutexQueueInt, int, mtds::TwoMutexQueue)(benchmark::State& state) { EnqueueOnceDequeue(state); }
BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueOnceDequeueMsQueueInt, int, mtds::MsQueue)(benchmark::State& state) { EnqueueOnceDequeue(state); }

BENCHMARK_REGISTER_F(QueueFixture, EnqueueDequeueTwoMutexQueueInt)->Arg(1'000'000)->ThreadRange(1, 16)->Unit(benchmark::kMillisecond)->UseRealTime();
BENCHMARK_REGISTER_F(QueueFixture, EnqueueDequeueMsQueueInt)->Arg(1'000'000)->ThreadRange(1, 16)->Unit(benchmark::kMillisecond)->UseRealTime();

BENCHMARK_REGISTER_F(QueueFixture, EnqueueDequeueOnceTwoMutexQueueInt)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();
BENCHMARK_REGISTER_F(QueueFixture, EnqueueDequeueOnceMsQueueInt)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();

BENCHMARK_REGISTER_F(QueueFixture, EnqueueOnceDequeueTwoMutexQueueInt)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();
BENCHMARK_REGISTER_F(QueueFixture, EnqueueOnceDequeueMsQueueInt)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();

// std::bad_alloc on MsQueue<std::string>

//BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueDequeueTwoMutexQueueString, std::string, mtds::TwoMutexQueue)(benchmark::State& state) { QueueFixture::EnqueueDequeue(state); }
//BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueDequeueMsQueueString, std::string, mtds::MsQueue)(benchmark::State& state) { EnqueueDequeue(state); }
//
//BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueDequeueOnceTwoMutexQueueString, std::string, mtds::TwoMutexQueue)(benchmark::State& state) { EnqueueDequeueOnce(state); }
//BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueDequeueOnceMsQueueString, std::string, mtds::MsQueue)(benchmark::State& state) { EnqueueDequeueOnce(state); }
//
//BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueOnceDequeueTwoMutexQueueString, std::string, mtds::TwoMutexQueue)(benchmark::State& state) { EnqueueOnceDequeue(state); }
//BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueOnceDequeueMsQueueString, std::string, mtds::MsQueue)(benchmark::State& state) { EnqueueOnceDequeue(state); }
//
//BENCHMARK_REGISTER_F(QueueFixture, EnqueueDequeueTwoMutexQueueString)->Arg(1'000'000)->ThreadRange(1, 16)->Unit(benchmark::kMillisecond)->UseRealTime();
//BENCHMARK_REGISTER_F(QueueFixture, EnqueueDequeueMsQueueString)->Arg(1'000'000)->ThreadRange(1, 16)->Unit(benchmark::kMillisecond)->UseRealTime();
//
//BENCHMARK_REGISTER_F(QueueFixture, EnqueueDequeueOnceTwoMutexQueueString)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();
//BENCHMARK_REGISTER_F(QueueFixture, EnqueueDequeueOnceMsQueueString)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();
//
//BENCHMARK_REGISTER_F(QueueFixture, EnqueueOnceDequeueTwoMutexQueueString)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();
//BENCHMARK_REGISTER_F(QueueFixture, EnqueueOnceDequeueMsQueueString)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();

BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueDequeueTwoMutexQueueMyPair, MyPair, mtds::TwoMutexQueue)(benchmark::State& state) { QueueFixture::EnqueueDequeue(state); }
BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueDequeueMsQueueMyPair, MyPair, mtds::MsQueue)(benchmark::State& state) { EnqueueDequeue(state); }

BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueDequeueOnceTwoMutexQueueMyPair, MyPair, mtds::TwoMutexQueue)(benchmark::State& state) { EnqueueDequeueOnce(state); }
BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueDequeueOnceMsQueueMyPair, MyPair, mtds::MsQueue)(benchmark::State& state) { EnqueueDequeueOnce(state); }

BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueOnceDequeueTwoMutexQueueMyPair, MyPair, mtds::TwoMutexQueue)(benchmark::State& state) { EnqueueOnceDequeue(state); }
BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, EnqueueOnceDequeueMsQueueMyPair, MyPair, mtds::MsQueue)(benchmark::State& state) { EnqueueOnceDequeue(state); }

BENCHMARK_REGISTER_F(QueueFixture, EnqueueDequeueTwoMutexQueueMyPair)->Arg(1'000'000)->ThreadRange(1, 16)->Unit(benchmark::kMillisecond)->UseRealTime();
BENCHMARK_REGISTER_F(QueueFixture, EnqueueDequeueMsQueueMyPair)->Arg(1'000'000)->ThreadRange(1, 16)->Unit(benchmark::kMillisecond)->UseRealTime();

BENCHMARK_REGISTER_F(QueueFixture, EnqueueDequeueOnceTwoMutexQueueMyPair)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();
BENCHMARK_REGISTER_F(QueueFixture, EnqueueDequeueOnceMsQueueMyPair)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();

BENCHMARK_REGISTER_F(QueueFixture, EnqueueOnceDequeueTwoMutexQueueMyPair)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();
BENCHMARK_REGISTER_F(QueueFixture, EnqueueOnceDequeueMsQueueMyPair)->Arg(1'000'000)->ThreadRange(2, 16)->Unit(benchmark::kMillisecond)->UseRealTime();

BENCHMARK_MAIN();
