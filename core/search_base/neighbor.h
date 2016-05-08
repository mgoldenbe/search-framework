///@file
///@brief INTERFACES CHECKED.

#ifndef STATE_NEIGHBOR_H
#define STATE_NEIGHBOR_H

template <typename State_, bool uniformFlag = true> struct StateNeighbor {
    using State = State_;
    using StateUniquePtr = std::unique_ptr<const State>;
    using CostType = typename State::CostType;
    StateNeighbor(State *s, CostType c) : scPair_(StateUniquePtr(s), c) {}
    StateUniquePtr &state() { return scPair_.first; }
    //const StateUniquePtr &state() const { return scPair_.first; }
    State stateCopy() const { return *(scPair_.first); }
    CostType cost() const { return scPair_.second; }

private:
    std::pair<StateUniquePtr, CostType> scPair_;
};

template <typename State_> struct StateNeighbor<State_, true> {
    using State = State_;
    using StateUniquePtr = std::unique_ptr<const State>;
    using CostType = typename State::CostType; // should be int
    StateNeighbor(State *s, CostType = CostType{1}) : state_(s) {}
    StateUniquePtr &state() { return state_; }
    State stateCopy() const { return *state_; }
    CostType cost() const { return 1; }

private:
    StateUniquePtr state_;
};

#endif
