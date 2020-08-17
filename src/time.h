#ifndef DRAGONFLY_TIME_H_
#define DRAGONFLY_TIME_H_

#include <chrono>

namespace df {

    class DfTime 
    {
    public:
        static uint64_t Now() {
            auto now  = std::chrono::high_resolution_clock::now();
            auto nano_time_pt = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
            auto epoch = nano_time_pt.time_since_epoch();
            uint64_t now_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch).count();
            return now_nano;
        }
    };
}

#endif