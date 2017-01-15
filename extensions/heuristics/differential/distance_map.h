#ifndef SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_DISTANCE_MAP_H
#define SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_DISTANCE_MAP_H

/// \file
/// \brief The differential heuristic's distance map
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace heuristic {
namespace differential {


struct NoIndex {
    static constexpr auto kind = IndexKind::none;
};

template <class State, class Index = NoIndex, IndexKind kind = Index::kind>
struct DistanceMap;

template <class State, class Index>
struct DistanceMap<State, Index, IndexKind::noInverse> {
    using CostType = typename State::CostType;
    using Distances = std::vector<CostType>;

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

    CostType distance(const State &s) const { return distances_[Index::to(s)]; }

    int nStates() const { return nStates_; }

protected:
    Distances distances_;
    int nStates_;
};

template <class State, class Index>
struct DistanceMap<State, Index, IndexKind::withInverse>
    : DistanceMap<State, Index, IndexKind::noInverse> {
    using Base = DistanceMap<State, Index, IndexKind::noInverse>;
    using CostType = typename Base::CostType;
    using Distances = typename Base::Distances;
    using Base::Base;
    using Base::distances_;

    using Iterator = core::util::VectorSkipIterator<Distances>;
    using ConstIterator = core::util::VectorSkipConstIterator<Distances>;
    Iterator begin() {
        return Iterator{distances_, distances_.begin(), CostType{-1}};
    }
    ConstIterator begin() const {
        return ConstIterator{distances_, distances_.begin(), CostType{-1}};
    }
    Iterator end() {
        return Iterator{distances_, distances_.end(), CostType{-1}};
    }
    ConstIterator end() const {
        return ConstIterator{distances_, distances_.end(), CostType{-1}};
    }
    template <class It>
    State itToState(It it) const {return Index::from(it.index());}

    template <class Distances1>
    void intersect(const Distances1 &d) {
        for (auto it = begin(); it != end(); ++it) {
            int index = it.index();
            *it = std::min(*it, d.distance(Index::from(index)));
        }
    }

    State farthest() const {
        auto maxIndex = std::max_element(begin(), end()).index();
        return Index::from(maxIndex);
    }
};

template <class State, class Index>
struct DistanceMap<State, Index, IndexKind::none> {
    using CostType = typename State::CostType;
    using Distances =
        std::unordered_map<State, CostType, core::util::StateHash<State>>;
    using Iterator = core::util::MapKeyIterator<Distances>;
    using ConstIterator = core::util::MapKeyConstIterator<Distances>;
    Iterator begin() { return distances_.begin(); }
    ConstIterator begin() const { return distances_.cbegin(); }
    Iterator end() { return distances_.end(); }
    ConstIterator end() const { return distances_.cend(); }
    template <class It> State itToState(It it) const { return it->first; }

    template <class MyAlg = algorithm::SimpleUniformCost>
    DistanceMap(const State &s) {
        distances_ = MyAlg::distanceMap(s);
        nStates_ = distances_.size();
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

    int nStates() const { return nStates_; }

protected:
    Distances distances_;
    int nStates_;
};

/// Pivot with distances to all the states.
/// \tparam State The state type representing the search domain.
template <class State, class Index = NoIndex>
struct Pivot : DistanceMap<State, Index> {
    Pivot(const State &s) : DistanceMap<State, Index>{s}, pivot_{s} {}

private:
    State pivot_;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
