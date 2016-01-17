
#ifndef EXPLICIT_STATE_H
#define EXPLICIT_STATE_H

#include <iostream>
#include <boost/functional/hash.hpp>
#include "Node.h"

template<class ExplicitSpace>
struct ExplicitState {
    using CostType = typename ExplicitSpace::CostType;
    using MyType = ExplicitState<ExplicitSpace>;
    using Neighbor = StateNeighbor<MyType>;
    using StateType = typename ExplicitSpace::StateType;

    ///@name Construction and Assignment
    //@{
    ExplicitState(const StateType &state) : state_(state) {}
    ExplicitState(const std::string &s) : state_(space_->state(s)) {}
    ExplicitState(const MyType &) = default;
    ExplicitState &operator=(const MyType &rhs) = default;
    //@}

    ///@name Read-Only Services
    //@{
    std::vector<Neighbor> successors() const {
        std::vector<Neighbor> res;
        for (auto &n: space_->neighbors(state_)) {
            Neighbor cur(new MyType(n));
            res.push_back(std::move(cur));
        }
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
private:
    StateType state_;
    static ExplicitSpace *space_;
};

template<class ExplicitSpace>
ExplicitSpace *ExplicitState<ExplicitSpace>::space_;

template <class ExplicitSpace>
bool
operator==(const std::shared_ptr<const ExplicitState<ExplicitSpace>> &lhs,
           const std::shared_ptr<const ExplicitState<ExplicitSpace>> &rhs) {
    return *lhs == *rhs;
}

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
