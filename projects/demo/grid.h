#ifndef SLB_CONFIG_GRID_H
#define SLB_CONFIG_GRID_H

#define SLB_STATE ext::domain::gridMap::GridMapState<>
#define SLB_COST_TYPE double
#define SLB_UNIFORM_DOMAIN false
#define SLB_COST_DIAGONAL 1.5
#define SLB_BASE_HEURISTIC OctileHeuristic<>

#endif
