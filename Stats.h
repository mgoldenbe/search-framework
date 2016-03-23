#ifndef STATS_H
#define STATS_H

#include <chrono>
#include "utilities.h"

struct Counter {
    Counter(const std::string &name) : name_(name) {}
    void operator++() { ++count_; }
    int get() const { return count_; }
    const std::string &name() const { return name_; }

    template <class Stream> Stream &dump(Stream &os) const {
        return os << count_;
    }
private:
    std::string name_;
protected:
    int count_ = 0;
};

// Important design feature: Timer is a Counter, so it can be handled uniformly
// with the rest of the stats.
struct Timer : Counter {
    using Counter::Counter;
    void start() { begin_ = std::chrono::system_clock::now(); }
    void stop() {
        count_ = std::chrono::duration_cast<std::chrono::microseconds>(
                       std::chrono::system_clock::now() - begin_).count();
    }

private:
    std::chrono::system_clock::time_point begin_;
};

struct TimerLock {
    TimerLock(Timer &timer) : timer_(timer) { timer_.start(); }
    ~TimerLock() { timer_.stop(); }
private:
    Timer &timer_;
};

using Stats = std::vector<Counter>;

// Assumes that rhs does not refer to lhs
Stats &operator+=(Stats &lhs, const Stats &rhs) {
    lhs.insert(lhs.end(), rhs.begin(), rhs.end());
    return lhs;
}
Stats operator+(const Stats &lhs, const Stats &rhs) {
    Stats res = lhs;
    res.insert(res.end(), rhs.begin(), rhs.end());
    return res;
}

#endif
