#ifndef GRID_MAP_STATE_H
#define GRID_MAP_STATE_H

#include "GridMap.h"
#include "ExplicitState.h"

template <typename CostType_, bool uniformFlag>
struct GridMapState: ExplicitState<GridMap<CostType_>> {
    using Base = ExplicitState<GridMap<CostType_>>;
    using Base::Base; // inherit constructors
    using Base::successors;

    using CostType = typename Base::CostType;
    using StateType = typename Base::StateType;
    using MyType = GridMapState<CostType, uniformFlag>;
    using Neighbor = StateNeighbor<MyType, uniformFlag>;

    std::vector<Neighbor> successors() const {
        return this->template successors<Neighbor>();
    }

    void visualLocation(double &x, double &y) const {
        x = this->space()->column(this->raw());
        y = this->space()->row(this->raw());
    }
};

template <typename CostType, bool uniformFlag>
bool operator==(
    const std::shared_ptr<const GridMapState<CostType, uniformFlag>> &lhs,
    const std::shared_ptr<const GridMapState<CostType, uniformFlag>> &rhs) {
    return *lhs == *rhs;
}

#endif
