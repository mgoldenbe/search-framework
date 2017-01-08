#ifndef SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_H
#define SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_H

/// \file
/// \brief The differential heuristic
/// \author Meir Goldenberg

namespace slb {
namespace ext {

namespace algorithm {
struct SimpleUniformCost;
}

namespace heuristic {

/// \namespace slb::ext::heuristic::differential
/// The differential heuristic
namespace differential {

/// Additions to the command line related to the differential heuristic.
struct CommandLine {
    /// Returns the number of pivots.
    /// \return The number of pivots.
    int nPivots() { return nPivots_.getValue(); }

private:
    /// Command line option for the number of pancakes.
    TCLAP::ValueArg<int> nPivots_;

protected:
    /// Injects this addition to the command line object.
    /// \param cmd The command-line object.
    CommandLine(TCLAP::CmdLine &cmd)
        : nPivots_("", "nPivots", "Number of pivots", false, -1, "int",
                     cmd) {}
};

template <class State> struct NoIndex;

template <class State, template <class> class Index = NoIndex>
struct DistancesToPivots;

template <class State>
struct DistancesToPivots<State, NoIndex> {
    using CostType = typename State::CostType;
    using MyMap = std::unordered_map<State, std::vector<CostType>,
                                     core::util::StateHash<State>>;
    using It = typename std::vector<CostType>::const_iterator;

    template <class MapType>
    void append(const MapType &m) {
        for (const auto &state: m) m_[state].push_back(m.distance(state));
        ++nPivots_;
    }

    /// Return a pair of iterators denoting the distances to all the pivots from the given state.
    It pivotIt(const State &s) const {
        auto it = m_.find(s);
        assert(it != m_.end());
        return (it->second).begin();
    }

    int nPivots() const { return nPivots_; }

    void clear() {
        nPivots_ = 0;
        m_.clear();
    }

private:
    int nPivots_ = 0;
    MyMap m_;
};

template <class State, template <class> class Index = NoIndex>
struct DistanceMap;

template <class State>
struct DistanceMap<State, NoIndex> {
    using CostType = typename State::CostType;
    using Distances =
        std::unordered_map<State, CostType, core::util::StateHash<State>>;
    using StateIterator = core::util::MapKeyIterator<Distances>;
    using StateConstIterator = core::util::MapKeyConstIterator<Distances>;
    StateIterator begin() { return distances_.begin(); }
    StateConstIterator begin() const { return distances_.cbegin(); }
    StateIterator end() { return distances_.end(); }
    StateConstIterator end() const { return distances_.cend(); }

    /// \tparam MyAlg Always \ref algorithm::SimpleUniformCost. It is a template
    /// parameter to avoid dependencies.
    template <class MyAlg = algorithm::SimpleUniformCost>
    DistanceMap(const State &s) {
        using MyInstance = core::sb::Instance<State>;
        auto instance = MyInstance(std::vector<State>{s}, std::vector<State>(1),
                                   MeasureSet{});
        MyAlg search(instance);
        search.run();
        distances_ = search.distanceMap();
    }

    CostType distance(const State &s) const {
        auto it = distances_.find(s);
        assert(it != distances_.end());
        return it->second;
    }

    template <class Distances1>
    void intersect(const Distances1 &d) {
        for (auto &el : distances_)
            el.second = std::min(el.second, d.distance(el.first));
    }

    State farthest() const { return core::util::mapMax(distances_).first; }

    const Distances &distanceMap() const { return distances_; }

protected:
    Distances distances_;
};

/// Pivot with distances to all the states.
/// \tparam State The state type representing the search domain.
template <class State, template <class> class Index = NoIndex>
struct Pivot : DistanceMap<State, Index> {
    Pivot(const State &s) : DistanceMap<State, Index>{s}, pivot_{s} {}

private:
    State pivot_;
};

struct Furthest {};

/// The differential heuristic.
/// \tparam State The state type representing the search domain.
template <class State, class BaseHeuristic,
          template <class> class Index = NoIndex>
struct HBase {
    using CostType = typename State::CostType;
    using Distances = DistancesToPivots<State, Index>;

    template <CMD_TPARAM>
    HBase(const State &goal)
        : nPivots_{CMD_T.nPivots()}, goal_{goal}, heuristic_(goal_) {}

    CostType operator()(const State &s) const {
        CostType res = heuristic_(s);
        if (nPivots_ == 0) return res;
        auto it = distances_.pivotIt(s);
        for (int i = 0; i != nPivots_; ++i) {
            auto myH = it[i] - goalToPivots_[i];
            if (myH < CostType{0}) myH = -myH;
            res = std::max(res, myH);
        }
        return res;
    }

    protected:
        int nPivots_;
        static Distances distances_;
        const State &goal_;
        std::vector<CostType> goalToPivots_;
        BaseHeuristic heuristic_;

        virtual void myMakePivots() = 0;
        void makePivots() {
            if (nPivots_ != distances_.nPivots()) {
                distances_.clear();
                this->myMakePivots();
            }
        }
        void fillGoalToPivots() {
            if (nPivots_ == 0) return;
            auto it = distances_.pivotIt(goal_);
            for (int i = 0; i != nPivots_; ++i) goalToPivots_.push_back(it[i]);
        }
};
template <class State, class BaseHeuristic, template <class> class Index>
typename HBase<State, BaseHeuristic, Index>::Distances
    HBase<State, BaseHeuristic, Index>::distances_;

/// The differential heuristic. Specializations are for particular placements.
template <class State = SLB_STATE,
          class BaseHeuristic = SLB_DIFFERENTIAL_BASE_HEURISTIC,
          class Placement = SLB_DIFFERENTIAL_PLACEMENT,
          template <class> class IndexWithInverse = SLB_DIFFERENTIAL_INDEX_WITH_INVERSE, // = NoIndex
          template <class> class CompactIndex = SLB_DIFFERENTIAL_COMPACT_INDEX,          // = IndexWithInverse
          template <class> class FastIndex = SLB_DIFFERENTIAL_FAST_INDEX>                // = IndexWithInverse
struct H;

/// The differential heuristic with the furthest placement.
/// \tparam State The state type representing the search domain.
template <class State, class BaseHeuristic,
          template <class> class IndexWithInverse,
          template <class> class CompactIndex,
          template <class> class FastIndex>
struct H<State, BaseHeuristic, Furthest, IndexWithInverse, CompactIndex, FastIndex>
    : HBase<State, BaseHeuristic, CompactIndex> {
    using MyBase = HBase<State, BaseHeuristic, CompactIndex>;
    using MyBase::nPivots_;
    using MyBase::distances_;

    H(const State &goal) : MyBase(goal) {
        this->makePivots();
        this->fillGoalToPivots();
    }

    virtual void myMakePivots() {
        // This has to have reverse to have farthest()
        DistanceMap<State, IndexWithInverse> toClosestPivot{firstPivot()};
        distances_.append(toClosestPivot);

        for (int i = 1; i != nPivots_; ++i) {
            DistanceMap<State, FastIndex> pivotMap{toClosestPivot.farthest()};
            distances_.append(pivotMap);
            if (i != nPivots_ - 1) toClosestPivot.intersect(pivotMap);
        }
    }

private:
    State firstPivot() {
        DistanceMap<State, IndexWithInverse> temp{State{}};
        return temp.farthest();
    }
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
