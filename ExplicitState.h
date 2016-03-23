#ifndef EXPLICIT_STATE_H
#define EXPLICIT_STATE_H

#include <iostream>
#include <boost/functional/hash.hpp>
#include "Node.h"

// The base class for a state in an explicit domain
template<class ExplicitSpace>
struct ExplicitState {
    using CostType = typename ExplicitSpace::CostType;
    using MyType = ExplicitState<ExplicitSpace>;
    using StateType = typename ExplicitSpace::StateType;

    ///@name Construction and Assignment
    //@{
    ExplicitState() : state_(space_->defaultState()) {}
    ExplicitState(const StateType &state) : state_(state) {}
    ExplicitState(const std::string &s) : state_(space_->state(s)) {}
    ExplicitState(const MyType &) = default;
    ExplicitState &operator=(const MyType &rhs) = default;
    //@}

    ///@name Read-Only Services
    //@{
    template<class Neighbor>
    std::vector<Neighbor> successors() const {
        std::vector<Neighbor> res;
        for (auto &n: space_->template neighbors<Neighbor>(state_))
            res.push_back(std::move(n));
        return res;
    }

    std::size_t hash() const {
        boost::hash<StateType> myHash;
        return myHash(state_);
    }

    const StateType &raw() const { return state_; }
    //@}

    template <typename charT>
    friend std::basic_ostream<charT> &operator<<(std::basic_ostream<charT> &o,
                                                 const MyType &rhs) {
        return o << rhs.space()->stateStr(rhs.state_);
    }

    friend bool operator==(const MyType &lhs, const MyType &rhs) {
        return lhs.state_ == rhs.state_;
    }

    static void space(ExplicitSpace *space) {space_ = space;}
    static const ExplicitSpace *space() {return space_;}
    static StateType random() { return space_->random(); }
private:
    StateType state_;
    static ExplicitSpace *space_;
};

template<class ExplicitSpace>
ExplicitSpace *ExplicitState<ExplicitSpace>::space_ = nullptr;

struct ManhattanHeuristic {
    template <class ExplicitSpace>
    typename ExplicitSpace::CostType
    operator()(const ExplicitState<ExplicitSpace> &s,
               const ExplicitState<ExplicitSpace> &goal) const {
        return ExplicitState<ExplicitSpace>::space()->manhattan(s.raw(),
                                                                goal.raw());
    }
};

#endif
