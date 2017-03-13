#ifndef SLB_EXTENSIONS_DOMAINS_GRID_FWD_H
#define SLB_EXTENSIONS_DOMAINS_GRID_FWD_H

/// \file
/// \brief Forward declarations for \ref extensions/domains/grid.h.
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace domain {
namespace grid {

template <typename CostType = SLB_COST_TYPE,
          bool uniformFlag = SLB_UNIFORM_DOMAIN>
struct Grid;

} // namespace
} // namespace
} // namespace
} // namespace

#endif
