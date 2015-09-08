#pragma once
#include <chrono>

namespace search
{
    //
    //  class Timing
    //  an RAII style timer using std::chrono in c++ 11
    //
    template<typename Milisecond>
    struct Timing
    {
        using Time = std::chrono::high_resolution_clock;
        using TimePoint = std::chrono::high_resolution_clock::time_point;

        explicit Timing(Milisecond& record) :
            recording{ record }, start{ Time::now() }
        { }

        ~Timing()
        {
            recording = std::chrono::duration<Milisecond>(Time::now() - start).count();
        }

        Milisecond& recording;
        TimePoint start;
    };
}