#ifndef STATE_NEIGHBOR_H
#define STATE_NEIGHBOR_H

/// \file
/// \brief The types for a single neighbor of a given state.
/// \author Meir Goldenberg

/// The type for a single neighbor of a given state. For maximal performance, a
/// specialization for the case of domain with uniform-cost actions is defined
/// separately.
/// \tparam State The state type, represents the domain.
/// \tparam uniformFlag Determines whether the domain is a uniform cost one or
/// not.
template <typename State_, bool uniformFlag = true> struct StateNeighbor {
    using State = State_; ///< The state type, represents the domain.

    /// The type representing the cost of actions in the domain.
    using CostType = typename State::CostType;

    /// Initializes the neighbor based on the neighbor state and cost
    /// of the action that leads to that state.
    /// \param s The neighbor state, which must be a right value.
    /// \param c Cost of the action that leads to \c s.
    StateNeighbor(State &&s, CostType c) : scPair_(s, c) {}

    /// Returns the neighbor state.
    /// \return Reference to the neighbor state.
    /// \note This reference must be non-const to allow moving the neighbor
    State &state() { return scPair_.first; }

    /// Returns cost of the action that leads to the neighbor state.
    /// \return Cost of the action that leads to the neighbor state.
    CostType cost() const { return scPair_.second; }

private:
    /// The state-cost pair representing the neighbor.
    std::pair<State, CostType> scPair_;
};

/// The type for a single neighbor of a given state of a domain with
/// uniform-cost actions.
/// \tparam State The state type, represents the domain.
template <typename State_> struct StateNeighbor<State_, true> {
    using State = State_; ///< The state type, represents the domain.

    /// The type representing the cost of actions in the domain.
    using CostType = typename State::CostType;

    /// Initializes the neighbor based on the neighbor state and cost
    /// of the action that leads to that state.
    /// \param s The neighbor state, must right value.
    /// \note The cost parameter is not used in this uniform case, but is
    /// present for uniformness of the interface.
    StateNeighbor(State &&s, CostType = CostType{1}) : state_(s) {}

    /// Returns the neighbor state.
    /// \return Reference to the neighbor state.
    /// \note This reference must be non-const to allow moving the neighbor
    /// state into a search node.
    State &state() { return state_; }

    /// Returns cost of the action that leads to the neighbor state, one in this
    /// case.
    /// \return Cost of the action that leads to the neighbor state, one in this
    /// case.
    CostType cost() const { return 1; }

private:
    State state_; ///< The neighbor state.
};

#endif
