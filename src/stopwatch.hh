#include <chrono>
#include <iostream>
#include <unistd.h>

template <typename Clock = std::chrono::steady_clock>
class Stopwatch
{
    typename Clock::time_point last_;

public:
    Stopwatch()
        : last_(Clock::now())
    {}
    
    void start()
    {
        *this = Stopwatch();
    }

    void reset()
    {
        *this = Stopwatch();
    }

    typename Clock::duration elapsed() const
    {
        return Clock::now() - last_;
    }
    
    typename Clock::duration tick()
    {
        auto now = Clock::now();
        auto elapsed = now - last_;
        last_ = now;
        return elapsed;
    }
};

template <typename T, typename Rep, typename Period>
T duration_cast(const std::chrono::duration<Rep, Period>& duration)
{
    return duration.count() * static_cast<T>(Period::num) / static_cast<T>(Period::den);
}
