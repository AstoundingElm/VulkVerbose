#pragma once
#include <sys/time.h>

#if _POSIX_C_SOURCE >= 199309L
#include <time.h>
#else 
#include <unistd.h>
#endif


f64 platformGetAbsoluteTime(){
        
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        return now.tv_sec + now.tv_nsec *  0.000000001;
}
struct kclock{
        
        f64 start_time;
        f64 elapsed;
};

void clockUpdate(struct kclock* clock) {
        if (clock->start_time != 0) {
                clock->elapsed = platformGetAbsoluteTime() - clock->start_time;
        }
}

void clockStart(struct kclock* clock) {
        clock->start_time = platformGetAbsoluteTime();
        clock->elapsed = 0;
}

void clockStop(struct kclock* clock) {
        clock->start_time = 0;
}