#ifndef SLB_CORE_UTIL_MEASURE_H
#define SLB_CORE_UTIL_MEASURE_H

/// \file
/// \brief The type for a single named performance measure and a timer derived
/// from it.
/// \author Meir Goldenberg

namespace slb {
namespace core {
namespace util {
std::string double2str(double f);

/// The type for a single named performance measure.
struct Measure {
    /// Initializes the measure with a name.
    /// \param name The name of the new measure.
    Measure(const std::string &name) : name_(name) {}

    /// Initializes the measure with a name and a value.
    /// \param name The name of the new measure.
    /// \param x The value of the new measure.
    Measure(const std::string &name, double x) : name_(name), x_(x) {}

    /// Increments the measure by 1.0.
    void operator++() { ++x_; }

    /// Increments the measure by the value stored by the right-hand side
    /// operand.
    /// \return Reference to the modified measure.
    Measure &operator+=(const Measure &rhs) {
        x_ += rhs.value();
        return *this;
    }

    /// Returns the measure's current value.
    /// \return The measure's current value.
    double value() const { return x_; }

    /// Returns the measure's name.
    /// \return The measure's name.
    const std::string &name() const { return name_; }

    /// Dumps the measure to the given stream.
    /// \tparam Stream The stream type.
    /// \param os The stream.
    /// \return The modified stream.
    template <class Stream> Stream &dump(Stream &os) const {
        return os << util::double2str(x_);
    }

    /// Sets the value of the measure to the given value.
    /// \param x The new value of the measure.
    void set(double x) { x_ = x; }

    /// Sets the name of the measure to the given name.
    /// \param name The new name of the measure.
    void setName(const std::string &name) { name_ = name; }

private:
    std::string name_; ///< Measure's name.
protected:
    double x_ = 0.0; ///< Measure's current value.
};

/// A special measure for timing.
/// \note Since \ref Timer is a \ref Measure, it can be handled uniformly
// with the rest of the measures.
struct Timer : Measure {
    using Measure::Measure; /// Inherit constructors.

    /// Starts timing.
    void start() { begin_ = std::chrono::system_clock::now(); }

    /// Sets the value of the measure with the time elapsed
    /// since the timing started. The time is measured in milliseconds.
    void stop() {
        x_ = static_cast<double>(
                 std::chrono::duration_cast<std::chrono::microseconds>(
                     std::chrono::system_clock::now() - begin_).count()) /
             1000;
    }

private:
    /// The point in time when \ref start was called.
    std::chrono::system_clock::time_point begin_;
};

/// Provides RAII mechanism so the authors of search algorithm implementations
/// should not need to worry about remembering to stop the timer for every
/// possible execution path.
struct TimerLock {
    /// Initializes the lock with the given timer and starts the timer.
    TimerLock(Timer &timer) : timer_(timer) { timer_.start(); }

    /// Stops the timer.
    ~TimerLock() { timer_.stop(); }
private:
    Timer &timer_; ///< The timer.
};

} // namespace
} // namespace
} // namespace

#endif
