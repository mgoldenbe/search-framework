#ifndef STATS_H
#define STATS_H

#include <chrono>

struct Measure {
    Measure(const std::string &name) : name_(name) {}
    void operator++() { ++x_; }
    Measure &operator+=(const Measure &rhs) {
        x_ += rhs.value();
        return *this;
    }
    double value() const { return x_; }
    const std::string &name() const { return name_; }

    template <class Stream> Stream &dump(Stream &os) const {
        return os << x_;
    }

    void set(double x) { x_ = x; }

private:
    std::string name_;
protected:
    double x_ = 0.0;
};

// Important design feature: Timer is a Counter, so it can be handled uniformly
// with the rest of the stats.
struct Timer : Measure {
    using Measure::Measure;
    void start() { begin_ = std::chrono::system_clock::now(); }
    void stop() {
        x_ = static_cast<double>(
                 std::chrono::duration_cast<std::chrono::microseconds>(
                     std::chrono::system_clock::now() - begin_).count()) /
             1000;
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

// Stats for one instance
struct MeasureSet {
    using iterator = std::vector<Measure>::iterator;
    using const_iterator = std::vector<Measure>::const_iterator;
    using reference = std::vector<Measure>::reference;
    using const_reference = std::vector<Measure>::const_reference;

    MeasureSet(std::initializer_list<Measure> list) : s_(list) {}
    iterator begin() { return s_.begin(); }
    iterator end() { return s_.end(); }
    const_iterator begin() const { return s_.begin(); }
    const_iterator end() const { return s_.end(); }
    int size() const { return s_.size(); }
    const_reference operator[](std::size_t n) const { return s_[n]; }

    void append(const Measure &m) { s_.push_back(m); }
    void prepend(const Measure &m) { s_.insert(s_.begin(), m); }
    void append(const MeasureSet &s) {
        for (auto &m : s) append(m);
    }
    MeasureSet &operator+=(const MeasureSet &rhs) {
        for (int i = 0; i < rhs.size(); i++) {
            assert(s_[i].name() == rhs[i].name());
            s_[i] += rhs[i];
        }
        return *this;
    }
    template <class Stream>
    Stream &dump(Stream &os, const std::string &prefixTitle = "", const std::string &prefixData = "", bool titleFlag = false) const {
        if (titleFlag) {
            os << prefixTitle;
            for (auto &m : s_) os << m.name();
            os << std::endl;
        }
        os << prefixData;
        for (auto &m : s_) os << m.value();
        os << std::endl;
        return os;
    }
private:
    std::vector<Measure> s_;
};

// Stats for instance set
struct Stats {
    using const_iterator = std::vector<MeasureSet>::const_iterator;
    using const_reference = std::vector<MeasureSet>::const_reference;

    Stats() = default;
    Stats(const MeasureSet &m) { append(m); }

    const const_iterator begin() const { return s_.begin(); }
    const const_iterator end() const { return s_.end(); }
    int size() const { return s_.size(); }
    const const_reference operator[](std::size_t n) const { return s_[n]; }
    void append(const MeasureSet &m, bool numberFlag = false) {
        MeasureSet mm = m;
        if (numberFlag) {
            Measure n("Num");
            n.set(s_.size());
            mm.prepend(n);
        }
        s_.push_back(mm);
    }
    Stats sum(bool averageCostFlag = false) const {
        MeasureSet res = s_[0];
        for (int i = 1; i < size(); i++)
            res += s_[i];
        if (averageCostFlag)
            for (auto &m : res)
                if (m.name() == "Cost") m.set(m.value() / size());
        return Stats(res);
    }
    Stats average() const {
        MeasureSet res = s_[0];
        for (int i = 1; i < size(); i++)
            res += s_[i];
        for (auto &m : res) m.set(m.value() / size());
        return Stats(res);
    }
    template <class Stream>
    Stream &dump(Stream &os, const std::string &prefixTitle = "",
                 const std::string &prefixData = "") const {
        s_[0].dump(os, prefixTitle, prefixData, true);
        for (int i = 1; i < size(); i++)
            s_[i].dump(os, prefixTitle, prefixData);
        return os;
    }

private:
    std::vector<MeasureSet> s_;
};

#endif
