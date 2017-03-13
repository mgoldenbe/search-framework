#ifndef SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_DISTANCES_TO_PIVOTS_H
#define SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_DISTANCES_TO_PIVOTS_H

/// \file
/// \brief The differential heuristic's distances to pivots.
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace heuristic {
namespace differential {

/// The type for working with distances to pivots.
/// The general definition is for the situation with an index.
/// The situation without an index is handled by a specialization.
/// \tparam State The state type representing the search domain.
/// \tparam Index Mapping of states to indices.
/// \tparam IndexKind The type denoting the kind of index.
template <class State, class Index = NoIndex, IndexKind k = Index::kind>
struct DistancesToPivots {
    /// The action cost type.
    using CostType = typename State::CostType;

    /// The type for storing distances to pivots.
    using Distances = std::vector<CostType>;

    /// The iterator for the distances to pivots.
    using It = typename std::vector<CostType>::const_iterator;

    /// Updates the expected number of pivots.
    void updateExpectedNPivots(int expectedNPivots) {
        expectedNPivots_ = expectedNPivots;
    }

    /// Appends the given distances to the existing ones.
    /// \tparam MyDistanceMap The type for storing the distances to be appended.
    /// \param myMap The distances to be appended.
    /// \note MyDistanceMap must provide begin() and end() associated with
    /// iterator over states, so it cannot be the DistanceMap with index without
    /// an inverse.
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

    /// Returns the iterator to the first distance to pivot from the given state.
    /// \param s The given state.
    /// \return The iterator to the first distance to pivot from \c s.
    It pivotIt(const State &s) const {
        return distances_.begin() + expectedNPivots_ * Index::to(s);
    }

    /// Returns the number of pivots.
    /// \return The number of pivots.
    int nPivots() const { return nPivots_; }

    /// Clears all the distances.
    void clear() {
        nPivots_ = 0;
        distances_.clear();
    }

private:
    int expectedNPivots_; ///< The expected number of pivots.
    int nPivots_ = 0;     ///< The number of pivots
                          ///< to which distances have been added.
    Distances distances_; ///< Distances to pivots.
};

/// The type for working with distances to pivots.
/// This specialization is for the situation without an index.
/// \tparam State The state type representing the search domain.
/// \tparam Index Mapping of states to indices.
/// \tparam IndexKind The type denoting the kind of index.
template <class State, class Index>
struct DistancesToPivots<State, Index, IndexKind::none> {
    /// The action cost type.
    using CostType = typename State::CostType;

    /// The type for storing distances to pivots.
    using Distances = std::unordered_map<State, std::vector<CostType>,
                                         core::util::StateHash<State>>;

    /// The iterator for the distances to pivots.
    using It = typename std::vector<CostType>::const_iterator;

    /// Appends the given distances to the existing ones.
    /// \tparam MyDistanceMap The type for storing the distances to be appended.
    /// \param myMap The distances to be appended.
    /// \note MyDistanceMap must provide begin() and end() associated with
    /// iterator over states, so it cannot be the DistanceMap with index without
    /// an inverse.
    template <class MyDistanceMap>
    void append(const MyDistanceMap &myMap) {
        for (const auto &state : myMap)
            distances_[state].push_back(myMap.distance(state));
        ++nPivots_;
    }

    /// Returns the iterator to the first distance to pivot from the given
    /// state.
    /// \param s The given state.
    /// \return The iterator to the first distance to pivot from \c s.
    It pivotIt(const State &s) const {
        auto it = distances_.find(s);
        assert(it != distances_.end());
        return (it->second).begin();
    }

    /// Returns the number of pivots.
    /// \return The number of pivots.
    int nPivots() const { return nPivots_; }

    /// Clears all the distances.
    void clear() {
        nPivots_ = 0;
        distances_.clear();
    }

private:
    int nPivots_ = 0; ///< The number of pivots
                      ///< to which distances have been added.
    Distances distances_; ///< Distances to pivots.
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
