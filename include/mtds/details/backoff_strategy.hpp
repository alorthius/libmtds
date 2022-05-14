#ifndef MTDS_BACKOFF_STRATEGY_HPP
#define MTDS_BACKOFF_STRATEGY_HPP

#include <cstddef>
#include <thread>

#define nop

namespace mtds::bos {

class exp_backoff {
public:
    explicit exp_backoff(size_t lower_bound = 8, size_t upper_bound = 8000, size_t step = 2) : lower_bound{lower_bound}, step{step}, upper_bound{upper_bound},
                                                                                               current{lower_bound} {}
    void operator()() {
        for (size_t i = 0; i < current; ++i) { nop; }
        current *= step;
        if (current > upper_bound)
            current = upper_bound;
    }
    void reset() { current = lower_bound; }

private:
    const size_t lower_bound;
    const size_t step;
    const size_t upper_bound;
    size_t current;
};

class yield_backoff {
public:
    explicit yield_backoff(size_t init = 4) : initial{init}, current{0} {}

    void operator()() {
        ++current;
        if (current >= initial) {
            std::this_thread::yield();
        }
    }
    void reset() { current = 0; }

private:
    const size_t initial;
    size_t current;
};

}

#endif //MTDS_BACKOFF_STRATEGY_HPP
