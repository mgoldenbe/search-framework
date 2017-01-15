#ifndef SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_DISTANCES_TO_PIVOTS_H
#define SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_DISTANCES_TO_PIVOTS_H

/// \file
/// \brief The differential heuristic's distances to pivots.
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace heuristic {
namespace differential {

// The general specialization is for the situation with an index. There are no
// specializations for kinds of index.
template <class State, class Index = NoIndex, IndexKind k = Index::kind>
struct DistancesToPivots {
    using CostType = typename State::CostType;
    using Distances = std::vector<CostType>;
    using It = typename std::vector<CostType>::const_iterator;

    void updateExpectedNPivots(int expectedNPivots) {
        expectedNPivots_ = expectedNPivots;
    }

    // MyDistanceMap must provide begin() and end() associated with iterator
    // over states, so it cannot be the DistanceMap with index without an
    // inverse.
    template <class MyDistanceMap>
    void append(const MyDistanceMap &myMap) {
        if (nPivots_ == 0)
            distances_.resize(expectedNPivots_ * Index::size());
        for (auto it = myMap.begin(); it != myMap.end(); ++it) {
            auto s = myMap.itToState(it);
            distances_[expectedNPivots_ * Index::to(s) + nPivots_] =
                myMap.distance(s);
        }
        ++nPivots_;
    }

    /// Return a pair of iterators denoting the distances to all the pivots from the given state.
    It pivotIt(const State &s) const {
        return distances_.begin() + expectedNPivots_ * Index::to(s);
    }

    int nPivots() const { return nPivots_; }

    void clear() {
        nPivots_ = 0;
        distances_.clear();
    }

private:
    int expectedNPivots_;
    int nPivots_ = 0;
    Distances distances_;
};

template <class State, class Index>
struct DistancesToPivots<State, Index, IndexKind::none> {
    using CostType = typename State::CostType;
    using Distances = std::unordered_map<State, std::vector<CostType>,
                                         core::util::StateHash<State>>;
    using It = typename std::vector<CostType>::const_iterator;

    template <class MyDistanceMap>
    void append(const MyDistanceMap &myMap) {
        for (const auto &state : myMap)
            distances_[state].push_back(myMap.distance(state));
        ++nPivots_;
    }

    /// Return a pair of iterators denoting the distances to all the pivots from the given state.
    It pivotIt(const State &s) const {
        auto it = distances_.find(s);
        assert(it != distances_.end());
        return (it->second).begin();
    }

    int nPivots() const { return nPivots_; }

    void clear() {
        nPivots_ = 0;
        distances_.clear();
    }

private:
    int nPivots_ = 0;
    Distances distances_;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
