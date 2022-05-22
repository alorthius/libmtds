#ifndef MTDS_EXAMPLE_BACK_OFF_HPP
#define MTDS_EXAMPLE_BACK_OFF_HPP

#include <cstddef>
#include <thread>

namespace mtds::back_off {

    class exp_backoff {
    public:
        explicit exp_backoff(size_t lower_bound = 8, size_t upper_bound = 8000, size_t step = 2) :
                lower_bound{lower_bound}, step{step}, upper_bound{upper_bound}, current{lower_bound}
                {}

        void operator()() {
            for (size_t i = 0; i < current; ++i) { asm("nop"); }
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
        explicit yield_backoff(size_t init = 10) : initial{init}, current{0} {}

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

}  // namespace mtds::back_off

#endif //MTDS_EXAMPLE_BACK_OFF_HPP
