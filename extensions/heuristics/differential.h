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

template <class State>
using DefaultDistanceMap =
    std::unordered_map<State, typename State::CostType, core::util::StateHash<State>>;

/// Pivot with distances to all the states.
/// \tparam State The state type representing the search domain.
/// \tparam DistanceMapT The type of the distance map. Ths parameter allows
/// obtaining improved performance for some search domains. A DistanceMap must
/// provide indexing by state and assignment from DefaultDistanceMap. It does
/// not necessarily store the states, since it may use hashing.
template <class State, template <class> class DistanceMapT = DefaultDistanceMap>
struct Pivot {
    using CostType = typename State::CostType;
    using DistanceMap = DistanceMapT<State>;

    /// \tparam MyAlg Always \ref algorithm::SimpleUniformCost. It is a template
    /// parameter to avoid dependencies.
    template <class MyAlg = algorithm::SimpleUniformCost>
    Pivot (const State &s) : pivot_(s) {
        using MyInstance = core::sb::Instance<State>;
        auto instance = MyInstance(std::vector<State>{pivot_},
                                   std::vector<State>(1), MeasureSet{});
        MyAlg search(instance);
        search.run();
        distances_ = search.distanceMap();
    }

    CostType distance(const State &s) const { return distances_.at(s); }

    const DistanceMap &distanceMap() const { return distances_; }

private:
    State pivot_;
    DistanceMap distances_;
};

struct Furthest {};

/// The differential heuristic.
/// \tparam State The state type representing the search domain.
template <class State, class BaseHeuristic, template <class> class DistanceMapT>
struct HBase {
    using CostType = typename State::CostType;

    template <CMD_TPARAM>
    HBase(const State &goal)
        : nPivots_{CMD_T.nPivots()}, goal_{goal}, heuristic_(goal) {}

    CostType operator()(const State &s) const {
        CostType res = heuristic_(s);
        for (const auto &p: pivots_) {
            auto myH = p.distance(s) - p.distance(goal_);
            if (myH < CostType{0}) myH = -myH;
            res = std::max(res, myH);
        }
        return res;
    }

    protected:
        int nPivots_;
        static std::vector<Pivot<State, DistanceMapT>> pivots_;
        const State &goal_;
        BaseHeuristic heuristic_;

        virtual void myMakePivots() = 0;
        void makePivots() {
            if (nPivots_ != pivots_.size()) {
                pivots_.clear();
                this->myMakePivots();
            }
        }
};
template <class State, class BaseHeuristic, template <class> class DistanceMapT>
std::vector<Pivot<State, DistanceMapT>>
    HBase<State, BaseHeuristic, DistanceMapT>::pivots_;

/// The differential heuristic. Specializations are for particular placements.
template <class State = SLB_STATE,
          class BaseHeuristic = SLB_DIFFERENTIAL_BASE_HEURISTIC,
          class Placement = SLB_DIFFERENTIAL_PLACEMENT,
          template <class> class DistanceMapT = DefaultDistanceMap>
struct H;

/// The differential heuristic with the furthest placement.
/// \tparam State The state type representing the search domain.
template <class State, class BaseHeuristic, template <class> class DistanceMapT>
struct H<State, BaseHeuristic, Furthest, DistanceMapT>
    : HBase<State, BaseHeuristic, DistanceMapT> {
    using MyBase = HBase<State, BaseHeuristic, DistanceMapT>;
    using MyBase::nPivots_;
    using MyBase::pivots_;
    using DistanceMap = DistanceMapT<State>;

    H(const State &goal) : MyBase(goal) { this->makePivots(); }
    virtual void myMakePivots() {
        Pivot<State> first{State{}}; // random auxiliary pivot, using default
                                     // distance map to get the vector
                                     // of states.
        std::vector<State> states = core::util::mapKeys(first.distanceMap());

        // The real first pivot.
        pivots_.emplace_back(core::util::mapMax(first.distanceMap()).first);

        DefaultDistanceMap<State> toClosestPivot;
        for (auto &el : states) toClosestPivot[el] = pivots_[0].distance(el);

        for (int i = 1; i != nPivots_; ++i) {
            pivots_.emplace_back(core::util::mapMax(toClosestPivot).first);
            // update toClosestPivot
            if (i == nPivots_ - 1) break;
            for (auto &el : toClosestPivot)
                el.second =
                    std::min(el.second, pivots_.back().distance(el.first));
        }
    }
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
