#ifndef SLB_EXTENSIONS_DOMAINS_GRID_H
#define SLB_EXTENSIONS_DOMAINS_GRID_H

/// \file
/// \brief The grid-based path finding domain.
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace domain {

/// \namespace slb::ext::domain::grid
/// \brief The grid map domain.
namespace grid {

/// Explicit state specific to the Grid Map domain.
/// \tparam CostType The type representing the cost of actions in the domain.
/// The default value is \c SLB_COST_TYPE.
/// \tparam uniformFlag Determines whether the domain is a uniform cost one or
/// not. The default value is \c SLB_UNIFORM_DOMAIN.
template <typename CostType, bool uniformFlag>
struct Grid : ExplicitState<explicit_space::Grid<CostType>> {
    using Base =
        ExplicitState<explicit_space::Grid<CostType>>; ///< The base class.
    using Base::Base;                                 ///< Inherit constructors

    /// This state type.
    using MyType = Grid<CostType, uniformFlag>;

    /// The type for representing a single neighbor state. Every domain
    /// must provide this name.
    using SNeighbor = StateNeighbor<MyType, uniformFlag>;

    /// Computes the neighbors of the state.
    /// \return Vector of neighbors of the state.
    std::vector<SNeighbor> stateSuccessors() const {
        return Base::template locationSuccessors<SNeighbor>();
    }

    /// Fills out the coordinates for the vertex representing the state.
    /// \param x The x-coordinate to be filled out.
    /// \param y The y-coordinate to be filled out.
    void visualLocation(double &x, double &y) const {
        this->space()->visualLocation(this->raw(), x, y);
    }

    /// Returns a random state.
    /// \return A random state.
    static Grid random() {
        return Grid(Base::random());
    }
};

/// The Manhattan heuristic functor.
/// \tparam State The type representing the domain.
template <class State = SLB_STATE>
struct ManhattanHeuristic {
    /// The constructor
    /// \param goal The goal state.
    ManhattanHeuristic(const State &goal) : goal_(goal) {}

    /// The function-call operator. Computes the heuristic.
    /// \param s The state from which the heuristic is to be computed.
    /// \return The heuristic function from \c s to \c goal.
    typename State::CostType operator()(const State &s) const {
        return State::space()->manhattan(s.raw(), goal_.raw());
    }

private:
    const State &goal_; ///< The goal state.
};

/// The octile heuristic functor.
/// \tparam State The type representing the domain.
template <class State>
struct OctileHeuristic {
    /// The constructor
    /// \param goal The goal state.
    OctileHeuristic(const State &goal) : goal_(goal) {}

    /// The function-call operator. Computes the heuristic.
    /// \param s The state from which the heuristic is to be computed.
    /// \return The heuristic function from \c s to \c goal.
    typename State::CostType operator()(const State &s) const {
        return State::space()->octile(s.raw(), goal_.raw());
    }

private:
    const State &goal_; ///< The goal state.
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
