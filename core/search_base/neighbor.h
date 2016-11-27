#ifndef SLB_CORE_SEARCH_BASE_NEIGHBOR_H
#define SLB_CORE_SEARCH_BASE_NEIGHBOR_H

/// \file
/// \brief The types for a single neighbor of a given state.
/// \author Meir Goldenberg

namespace slb {
namespace core {
namespace sb {

/// The type for storing the cost of a neighbor.
/// \tparam State The state type, represents the domain.
/// \tparam uniformFlag Determines whether the domain is a uniform cost one or
/// not.
template <typename State_, bool uniformFlag = true> struct Cost {
    using State = State_; ///< The state type, represents the domain.
    using CostType = typename State::CostType; ///< Action cost type.

    /// Constructor.
    /// \param c The cost.
    Cost(CostType c): cost_(c) {}

    /// Returns cost of the action that leads to the neighbor state.
    /// \return Cost of the action that leads to the neighbor state.
    CostType cost() const { return cost_; }
private:
    CostType cost_; ///< Action cost.
};

/// Specialization of the type for storing the cost of a neighbor for uniform
/// domains.
/// \tparam State The state type, represents the domain.
/// \tparam uniformFlag Determines whether the domain is a uniform cost one or
/// not.
template <typename State_> struct Cost<State_, true> {
    using State = State_; ///< The state type, represents the domain.
    using CostType = typename State::CostType; ///< Action cost type.

    /// Constructor.
    Cost(CostType) {}

    /// Returns cost of the action that leads to the neighbor state.
    /// \return Cost of the action that leads to the neighbor state.
    constexpr CostType cost() const { return CostType{1}; }
};



/// The type for a state neighbor of a given state.
/// \tparam State The state type, represents the domain.
/// \tparam uniformFlag Determines whether the domain is a uniform cost one or
/// not.
template <typename State_, bool uniformFlag = true>
struct StateNeighbor: Cost<State_, uniformFlag>  {
    using State = State_; ///< The state type, represents the domain.
    using MyCost =
        Cost<State_, uniformFlag>; ///< The type for storing the neighbor cost.
    using CostType = typename State::CostType; ///< Action cost type.

    /// Initializes the neighbor based on the neighbor state and cost
    /// of the action that leads to that state.
    /// \param s The neighbor state, which must be a right value.
    /// \param c Cost of the action that leads to \c s.
    StateNeighbor(State &&s, CostType c = CostType{1}) : MyCost(c), s_{std::move(s)} {}

    /// Returns the neighbor state.
    /// \return Reference to the neighbor state.
    /// \note This reference must be non-const to allow moving the neighbor
    State &state() { return s_; }

private:
    State s_; ///< The neighbor state.
};

/// The type for an action neighbor of a given state.
/// \tparam State The state type, represents the domain.
/// \tparam uniformFlag Determines whether the domain is a uniform cost one or
/// not.
template <typename State_, bool uniformFlag = true>
struct ActionNeighbor: Cost<State_, uniformFlag>  {
    using State = State_; ///< The state type, represents the domain.
    using MyCost = Cost<State_, uniformFlag>; ///< Action cost type.
    using Action = typename State::Action; ///< Action type.
    using CostType = typename State::CostType; ///< Action cost type.

    /// Initializes the neighbor based on the neighbor state and cost
    /// of the action that leads to that state.
    /// \param a The action, which must be a right value.
    /// \param c Cost of the action that leads to \c s.
    ActionNeighbor(Action &&a, CostType c = CostType{1}) : MyCost(c), a_{std::move(a)} {}
    /// Returns the action.
    /// \return Const reference to the action.
    const Action &action() const { return a_; }

private:
    Action a_; ///< The action.
};

} // namespace
} // namespace
} // namespace

#endif
