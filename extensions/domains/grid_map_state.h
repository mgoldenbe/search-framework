#ifndef GRID_MAP_STATE_H
#define GRID_MAP_STATE_H

/// \file
/// \brief The \ref GridMapState class.
/// \author Meir Goldenberg

namespace Domains {

/// Explicit state specific to the Grid Map domain.
/// \tparam CostType The type representing the cost of actions in the domain.
/// \tparam uniformFlag Determines whether the domain is a uniform cost one or
/// not.
template <typename CostType, bool uniformFlag>
struct GridMapState : ExplicitState<GridMap<CostType>> {
    using Base = ExplicitState<GridMap<CostType>>; ///< The base class.
    using Base::Base;                             ///< Inherit constructors

    /// This state type.
    using MyType = GridMapState<CostType, uniformFlag>;

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
        x = this->space()->column(this->raw());
        y = this->space()->row(this->raw());
    }

    /// Returns a random state.
    /// \return A random state.
    static GridMapState random() {
        return GridMapState(Base::random());
    }
};

}

#endif
