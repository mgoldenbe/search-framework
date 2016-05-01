///@file
///@brief INTERFACES CHECKED.

#ifndef UTIL_TIME_H
#define UTIL_TIME_H

// http://cc.byexamples.com/2007/05/25/nanosleep-is-better-than-sleep-and-usleep/
void __nsleep(const struct timespec *req, struct timespec *rem) {
    struct timespec temp_rem;
    if (nanosleep(req, rem) == -1)
        __nsleep(rem, &temp_rem);
}

int msleep(unsigned long milisec) {
    struct timespec req = {0,0}, rem = {0,0};
    time_t sec = (int)(milisec / 1000);
    milisec = milisec - (sec * 1000);
    req.tv_sec = sec;
    req.tv_nsec = milisec * 1000000L;
    __nsleep(&req, &rem);
    return 1;
}

#endif
