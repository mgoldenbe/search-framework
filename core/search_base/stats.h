#ifndef SLB_CORE_SEARCH_BASE_STATS_H
#define SLB_CORE_SEARCH_BASE_STATS_H

/// \file
/// \brief Facilities for accumulating statistics about the search algorithms'
/// performance.
/// \author Meir Goldenberg

namespace slb {
namespace core {
namespace sb {
using util::Measure;
using util::Timer;
using util::TimerLock;

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

    /// Dumps the measure titles to the given stream.
    /// \tparam Stream The stream type.
    /// \param os The stream.
    /// \param prefixTitle The string to prefix the title row with.
    /// \return The modified stream.
    template <class Stream>
    Stream &dumpTitle(Stream &os, const std::string &prefixTitle = "") const {
        if (prefixTitle.size()) os << prefixTitle;
        for (auto &m : s_) os << m.name();
        return (os << std::endl);
    }

    /// Dumps the measures' values to the given stream.
    /// \tparam Stream The stream type.
    /// \param os The stream.
    /// \param prefixData The string to prefix the data row with.
    /// \return The modified stream.
    template <class Stream>
    Stream &dumpValues(Stream &os, const std::string &prefixData = "") const {
        if (prefixData.size()) os << prefixData;
        for (auto &m : s_) m.dump(os);
        return os;
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
        if (titleFlag) os = dumpTitle(os, prefixTitle);
        dumpValues(os, prefixData);
        return (os << std::endl);
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
    /// \return Reference to the measure set for the problem instance with
    /// the index \c n.
    MeasureSet &operator[](std::size_t n) { return s_[n]; }

    /// Returns measure set for the problem instance with the given index.
    /// \return Const reference to the measure set for the problem instance with
    /// the index \c n.
    const MeasureSet &operator[](std::size_t n) const { return s_[n]; }

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

} // namespace
} // namespace
} // namespace

#endif
