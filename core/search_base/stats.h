#ifndef STATS_H
#define STATS_H

/// \file
/// \brief Facilities for accumulating statistics about the search algorithms'
/// performance.
/// \author Meir Goldenberg

/// The type for a single named performance measure.
struct Measure {
    /// Initializes the measure with a name.
    /// \param name The name of the new measure.
    Measure(const std::string &name) : name_(name) {}

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
        return os << x_;
    }

    /// Sets the value of the measure to the given value.
    /// \param x The new value of the measure.
    void set(double x) { x_ = x; }

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

    /// Stops timing and sets the value of the measure with the time elapsed
    /// since the timing started.
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

/// All the measures for a single problem instance.
/// \note The word "set" here does not have its usual mathematical connotation of being non-order-preserving.
struct MeasureSet {
    /// Iterator for \ref MeasureSet.
    using iterator = std::vector<Measure>::iterator;

    /// Const iterator for \ref MeasureSet.
    using const_iterator = std::vector<Measure>::const_iterator;

    /// Needed to enable range-based for-loop with a reference loop variable.
    using reference = std::vector<Measure>::reference;

    /// Needed to enable range-based for-loop with a const-reference loop
    /// variable.
    using const_reference = std::vector<Measure>::const_reference;

    /// Initializes the measure-set based on an initializer list of measures.
    MeasureSet(std::initializer_list<Measure> list) : s_(list) {}

    /// Needed to enable range-based loop.
    /// \return The \c begin iterator.
    iterator begin() { return s_.begin(); }

    /// Needed to enable range-based loop.
    /// \return The \c end iterator.
    iterator end() { return s_.end(); }

    /// Needed to enable range-based for-loop with a const loop variable.
    /// \return The \c begin const iterator.
    const_iterator begin() const { return s_.begin(); }

    /// Needed to enable range-based for-loop with a const loop variable.
    /// \return The \c end const iterator.
    const_iterator end() const { return s_.end(); }

    /// Returns the number of measures in the set.
    /// \return The number of measures in the set.
    int size() const { return s_.size(); }

    /// Returns the measure with the given index.
    /// \return Const reference to the measure with the index \c n.
    const_reference operator[](std::size_t n) const { return s_[n]; }

    /// Appends the given measure to the set.
    /// \param m The measure to append.
    void append(const Measure &m) { s_.push_back(m); }

    /// Inserts the given measure to the beginning of the set.
    /// \param m The measure to insert.
    void prepend(const Measure &m) { s_.insert(s_.begin(), m); }

    /// Appends all measures in the given set to the set.
    /// \param s The set to append.
    MeasureSet &append(const MeasureSet &s) {
        for (auto &m : s) append(m);
        return *this;
    }

    /// Adds the values of the measures from the right-hand side operand to the
    /// corresponding values in the set.
    /// \param rhs The set the values of whose measures are to be added.
    /// \return Reference to the modified set.
    /// \pre \c rhs contains the same number of measures as the set.
    MeasureSet &operator+=(const MeasureSet &rhs) {
        for (int i = 0; i < rhs.size(); i++) {
            assert(s_[i].name() == rhs[i].name());
            s_[i] += rhs[i];
        }
        return *this;
    }

    /// Dumps the set to the given stream.
    /// \tparam Stream The stream type.
    /// \param os The stream.
    /// \param prefixTitle The string to prefix the title row with.
    /// \param prefixData The string to prefix the data row with.
    /// \param titleFlag The title row will be output only if this flag is \c
    /// true.
    /// \return The modified stream.
    template <class Stream>
    Stream &dump(Stream &os, const std::string &prefixTitle = "",
                 const std::string &prefixData = "",
                 bool titleFlag = false) const {
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
    std::vector<Measure> s_; ///< The vector of measures in the set.
};

/// Performance statistics of a search algorithm for a set of problem instances.
struct Stats {
    /// Const iterator for \ref Stats.
    using const_iterator = std::vector<MeasureSet>::const_iterator;

    /// Needed to enable range-based for-loop with a const-reference loop
    /// variable.
    using const_reference = std::vector<MeasureSet>::const_reference;

    /// Default constructor; initializes empty stats.
    Stats() = default;

    /// Initializes stats with a measure set for a single instance.
    Stats(const MeasureSet &m) { append(m); }

    /// Needed to enable range-based for-loop with a const loop variable.
    /// \return The \c begin const iterator.
    const const_iterator begin() const { return s_.begin(); }

    /// Needed to enable range-based loop.
    /// \return The \c end iterator.
    const const_iterator end() const { return s_.end(); }

    /// Returns the number of instances for which measure sets are stored.
    /// \return The number of instances for which measure sets are stored.
    int size() const { return s_.size(); }

    /// Returns measure set for the problem instance with the given index.
    /// \return Const reference to the measure set for the problem instance with
    /// the index \c n.
    const const_reference operator[](std::size_t n) const { return s_[n]; }

    /// Appends the given measure set to the stats.
    /// \param m The measure set to append.
    /// \param numberFlag If \c true, the measure expressing the index of the
    /// problem instance will be inserted to the beginning of \c m before
    /// appending \c m to stats.
    void append(const MeasureSet &m, bool numberFlag = false) {
        MeasureSet mm = m;
        if (numberFlag) {
            Measure n("Num");
            n.set(s_.size());
            mm.prepend(n);
        }
        s_.push_back(mm);
    }

    /// Computes the aggregated measure set which summarizes all the stats for
    /// all the instances. Sum is used as the aggregate function.
    /// \param averageCostFlag If \c true, the measures with the name "Cost" are
    /// treated specially in that their values are averaged.
    /// \return Stats consisting of a single measure set that summarizes all the
    /// stats for all the instances.
    Stats sum(bool averageCostFlag = false) const {
        MeasureSet res = s_[0];
        for (int i = 1; i < size(); i++)
            res += s_[i];
        if (averageCostFlag)
            for (auto &m : res)
                if (m.name() == "Cost") m.set(m.value() / size());
        return Stats(res);
    }

    /// Computes the aggregated measure set which summarizes all the stats for
    /// all the instances. Average is used as the aggregate function.
    /// \return Stats consisting of a single measure set that summarizes all the
    /// stats for all the instances.
    Stats average() const {
        MeasureSet res = sum()[0];
        for (auto &m : res) m.set(m.value() / size());
        return Stats(res);
    }

    /// Dumps the stats to the given stream.
    /// \tparam Stream The stream type.
    /// \param os The stream.
    /// \param prefixTitle The string to prefix the title row with.
    /// \param prefixData The string to prefix each data row with.
    /// \return The modified stream.
    template <class Stream>
    Stream &dump(Stream &os, const std::string &prefixTitle = "",
                 const std::string &prefixData = "") const {
        s_[0].dump(os, prefixTitle, prefixData, true);
        for (int i = 1; i < size(); i++)
            s_[i].dump(os, prefixTitle, prefixData);
        return os;
    }

private:
    /// The measure sets for the solved problem instances.
    std::vector<MeasureSet> s_;
};

#endif
