#ifndef SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_H
#define SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_H

/// \file
/// \brief The differential heuristic
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace heuristic {

/// \namespace slb::ext::heuristic::differential
/// The differential heuristic
namespace differential {

template <class State>
using DefaultDistanceMap =
    std::unordered_map<State, typename State::CostType, util::StateHash<State>>;

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

    Pivot (const State &s) : pivot_(s) {
        using MyInstance = Instance<State>;
        auto instance = MyInstance(std::vector<State>(1){pivot_},
                                   std::vector<State>(1), MeasureSet{});
        algorithm::SimpleUniformSearch search;
        search.run();
        distances = search.distanceMap();
    }

    CostType distance(const State &s) const { return distances[s]; }

    const DistanceMap &distanceMap() const { return distanceMap; }

private:
    State pivot_;
    DistanceMap distances;
};

struct Furthest {};

/// The differential heuristic.
/// \tparam State The state type representing the search domain.
template <class State, template BaseHeuristic = SLB_DIFFERENTIAL_BASE_HEURISTIC,
          template <class> class DistanceMapT>
struct HBase {
    template <CMD_TPARAM>
    HBase(const State &goal)
        : nPivots{CMD_T.nPivots()}, goal_{goal}, heuristic_(goal) {
        // This can be made more efficient with the dynamic approach,
        // but that requires optionally keeping closest distance to pivots.
        if (nPivots != pivots.size()) {
            pivots.clear();
            makePivots();
        }
    }

    CostType operator(const State &s) {
        CostType res = heuristic_{goal};
        for (const auto &p: pivots) {
            auto myH = pivot.distance[s] - pivot.distance[goal_];
            if (myH < CostType{0}) myH = -myH;
            res = std::min(res, myH);
        }
        return res;
    }

    private:
        int nPivots;
        static std::vector < Pivot<State, DistanceMapT> pivots;
        const State &goal_;
        BaseHeuristic heuristic_;

        virtual void makePivots();
};

template <class State, template BaseHeuristic = SLB_DIFFERENTIAL_BASE_HEURISTIC,
          class Placement = SLB_DIFFERENTIAL_PLACEMENT,
          template <class> class DistanceMapT = DefaultDistanceMap>
struct H;

/// The differential heuristic with the furthest placement.
/// \tparam State The state type representing the search domain.
template <class State, template BaseHeuristic,
          template <class> class DistanceMapT>
struct H<State, BaseHeuristic, Furthest, DistanceMapT>
    : HBase<State, BaseHeuristic, DistanceMapT> {
    using HBase;
    using nPivots;
    using pivots;
    using DistanceMap = DistanceMapT<State>;

    virtual void makePivots() {
        Pivot<State> first{State{}}; // random auxiliary pivot, using default
                                     // distance map to get the vector
                                     // of states.
        std::vector<State> states = core::util::mapKeys(first.distanceMap());

        // The real first pivot.
        pivots.emplace_back(core::util::get_max(first.distanceMap()).first);

        DefaultDistanceMap toClosestPivot;
        for (auto &el : states) toClosestPivot[el] = pivots[0].distance[el];

        for (int i = 1; i != nPivots; ++i) {
            pivots.emplace_back(core::util::get_max(toClosestPivot()).first);
            // update toClosestPivot
            if (i == nPivots - 1) break;
            for (auto &el : toClosestPivot)
                el.second =
                    std::min(el.second, pivots.back().distanceMap()[el.first]);
        }
    }
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
