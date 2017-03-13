#ifndef SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_DISTANCE_MAP_H
#define SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_DISTANCE_MAP_H

/// \file
/// \brief The differential heuristic's distance map
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace heuristic {
namespace differential {

/// The type to denote the situation with no index.
struct NoIndex {
    /// The kind of index.
    static constexpr auto kind = IndexKind::none;
};

/// The type for storing distances to pivots.
/// The different kinds of index functions are handled by specializations.
/// \tparam State The state type representing the search domain.
/// \tparam Index Mapping of states to indices.
/// \tparam IndexKind The type denoting the kind of index.
template <class State, class Index = NoIndex, IndexKind kind = Index::kind>
struct DistanceMap;

/// The type for storing distances to pivots. This specialization is for an
/// index function without an inverse.
/// \tparam State The state type representing the search domain.
/// \tparam Index Mapping of states to indices.
/// \tparam IndexKind The type denoting the kind of index.
template <class State, class Index>
struct DistanceMap<State, Index, IndexKind::noInverse> {
    /// The action cost type.
    using CostType = typename State::CostType;

    /// The underlying type for storing distances to pivots.
    using Distances = std::vector<CostType>;

    /// The constructor.
    /// \tparam MyAlg The algorithm to be used to find distances to the
    /// specified state.
    /// \param s The state to which distances are to be stored.
    template <class MyAlg = algorithm::SimpleUniformCost>
    DistanceMap(const State &s) {
        auto myMap = MyAlg::distanceMap(s);
        nStates_ = myMap.size();
        for (const auto &el: myMap) {
            int myIndex = Index::to(el.first);
            if (myIndex > (int)distances_.size() - 1) distances_.resize(myIndex + 1, CostType{-1});
            distances_[myIndex] = el.second;
        }
    }

    /// Returns the distance from the given state.
    /// \param s The state of interest.
    /// \return The distance from \c s.
    CostType distance(const State &s) const { return distances_[Index::to(s)]; }

    /// Returns the number of states from which distances are stored.
    /// \return The number of states from which distances are stored.
    int nStates() const { return nStates_; }

protected:
    Distances distances_; ///< The stored distances.
    int nStates_; ///< the number of states from which distances are stored.
};

/// The type for storing distances to pivots.
/// This specialization is for the case of an index function with an inverse.
/// \tparam State The state type representing the search domain.
/// \tparam Index Mapping of states to indices.
/// \tparam IndexKind The type denoting the kind of index.
template <class State, class Index>
struct DistanceMap<State, Index, IndexKind::withInverse>
    : DistanceMap<State, Index, IndexKind::noInverse> {
    /// The direct base type.
    using Base = DistanceMap<State, Index, IndexKind::noInverse>;

    /// The action cost type.
    using CostType = typename Base::CostType;

    /// The underlying type for storing distances to pivots.
    using Distances = typename Base::Distances;

    using Base::Base;
    using Base::distances_;

    /// The iterator for the distances.
    using Iterator = core::util::VectorSkipIterator<Distances>;

    /// The constant iterator for the distances.
    using ConstIterator = core::util::VectorSkipConstIterator<Distances>;

    /// Returns iterator to the beginning of the distances.
    /// \return Iterator to the beginning of the distances.
    Iterator begin() {
        return Iterator{distances_, distances_.begin(), CostType{-1}};
    }

    /// Returns constant iterator to the beginning of the distances.
    /// \return Constant iterator to the beginning of the distances.
    ConstIterator begin() const {
        return ConstIterator{distances_, distances_.begin(), CostType{-1}};
    }

    /// Returns iterator to the end of the distances.
    /// \return Iterator to the end of the distances.
    Iterator end() {
        return Iterator{distances_, distances_.end(), CostType{-1}};
    }

    /// Returns constant iterator to the end of the distances.
    /// \return Constant iterator to the end of the distances.
    ConstIterator end() const {
        return ConstIterator{distances_, distances_.end(), CostType{-1}};
    }

    /// Returns the state (from which a distance is stored) corresponding to the
    /// given iterator.
    /// \tparam It The iterator type.
    /// \param it The given iterator.
    /// \return the state (from which a distance is stored) corresponding to \c
    /// it
    template <class It>
    State itToState(It it) const {return Index::from(it.index());}

    /// Computes the minimum of each stored distance and the corresponding
    /// distance given by the argument.
    /// \tparam Distances1 The type for the distances given by the argument.
    /// \param d The given distances.
    template <class Distances1>
    void intersect(const Distances1 &d) {
        for (auto it = begin(); it != end(); ++it) {
            int index = it.index();
            *it = std::min(*it, d.distance(Index::from(index)));
        }
    }

    /// Returns the state from which the distance is maximal.
    /// \return The state from which the distance is maximal.
    State farthest() const {
        auto maxIndex = std::max_element(begin(), end()).index();
        return Index::from(maxIndex);
    }
};

/// The type for storing distances to pivots.
/// This specialization is for the situation without an index.
/// \tparam State The state type representing the search domain.
/// \tparam Index Mapping of states to indices.
/// \tparam IndexKind The type denoting the kind of index.
template <class State, class Index>
struct DistanceMap<State, Index, IndexKind::none> {
    /// The action cost type.
    using CostType = typename State::CostType;

    /// The underlying type for storing distances to pivots.
    using Distances =
        std::unordered_map<State, CostType, core::util::StateHash<State>>;

    /// The iterator for the distances.
    using Iterator = core::util::MapKeyIterator<Distances>;

    /// The constant iterator for the distances.
    using ConstIterator = core::util::MapKeyConstIterator<Distances>;

    /// Returns iterator to the beginning of the distances.
    /// \return Iterator to the beginning of the distances.
    Iterator begin() { return distances_.begin(); }

    /// Returns constant iterator to the beginning of the distances.
    /// \return Constant iterator to the beginning of the distances.
    ConstIterator begin() const { return distances_.cbegin(); }

    /// Returns iterator to the end of the distances.
    /// \return Iterator to the end of the distances.
    Iterator end() { return distances_.end(); }

    /// Returns constant iterator to the end of the distances.
    /// \return Constant iterator to the end of the distances.
    ConstIterator end() const { return distances_.cend(); }

    /// Returns the state (from which a distance is stored) corresponding to the
    /// given iterator.
    /// \tparam It The iterator type.
    /// \param it The given iterator.
    /// \return the state (from which a distance is stored) corresponding to \c
    /// it
    template <class It> State itToState(It it) const { return it->first; }

    /// The constructor.
    /// \tparam MyAlg The algorithm to be used to find distances to the
    /// specified state.
    /// \param s The state to which distances are to be stored.
    template <class MyAlg = algorithm::SimpleUniformCost>
    DistanceMap(const State &s) {
        distances_ = MyAlg::distanceMap(s);
        nStates_ = distances_.size();
    }

    /// Returns the distance from the given state.
    /// \param s The state of interest.
    /// \return The distance from \c s.
    CostType distance(const State &s) const {
        auto it = distances_.find(s);
        assert(it != distances_.end());
        return it->second;
    }

    /// Computes the minimum of each stored distance and the corresponding
    /// distance given by the argument.
    /// \tparam Distances1 The type for the distances given by the argument.
    /// \param d The given distances.
    template <class Distances1>
    void intersect(const Distances1 &d) {
        for (auto &el : distances_)
            el.second = std::min(el.second, d.distance(el.first));
    }

    /// Returns the state from which the distance is maximal.
    /// \return The state from which the distance is maximal.
    State farthest() const { return core::util::mapMax(distances_).first; }

    /// Returns the raw distances.
    /// \return The raw distances.
    const Distances &distanceMap() const { return distances_; }

    /// Returns the number of states from which distances are stored.
    /// \return The number of states from which distances are stored.
    int nStates() const { return nStates_; }

protected:
    Distances distances_; ///< The stored distances.
    int nStates_; ///< the number of states from which distances are stored.
};

/// Pivot with distances to all the states.
/// \tparam State The state type representing the search domain.
/// \tparam Index Mapping of states to indices.
template <class State, class Index = NoIndex>
struct Pivot : DistanceMap<State, Index> {
    /// The constructor.
    /// \param s The pivot state.
    Pivot(const State &s) : DistanceMap<State, Index>{s}, pivot_{s} {}

private:
    State pivot_; ///< The pivot state.
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
