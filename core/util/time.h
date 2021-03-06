#ifndef SLB_CORE_UTIL_TIME_H
#define SLB_CORE_UTIL_TIME_H

///@file
///@brief Time-related utilities.
/// \author Meir Goldenberg

namespace slb {
namespace core {
namespace util {

/// Sleep for the given amount of time, while dealing with interrupts.
/// \param req Amount of time to sleep.
/// \param rem Remaining amount of time to sleep after the interrupt.
/// \note The code is taken from http://cc.byexamples.com/2007/05/25/nanosleep-is-better-than-sleep-and-usleep/
void __nsleep(const struct timespec *req, struct timespec *rem) {
    struct timespec temp_rem;
    if (nanosleep(req, rem) == -1)
        __nsleep(rem, &temp_rem);
}

/// Sleep for the given amount of time, while dealing with interrupts.
/// \param milisec Amount of time to sleep in milliseconds.
/// \note The code is taken from http://cc.byexamples.com/2007/05/25/nanosleep-is-better-than-sleep-and-usleep/
int msleep(unsigned long milisec) {
    struct timespec req = {0,0}, rem = {0,0};
    time_t sec = (int)(milisec / 1000);
    milisec = milisec - (sec * 1000);
    req.tv_sec = sec;
    req.tv_nsec = milisec * 1000000L;
    __nsleep(&req, &rem);
    return 1;
}

} // namespace
} // namespace
} // namespace

#endif
