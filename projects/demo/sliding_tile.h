#ifndef SLB_PROJECTS_DEMO_SLIDING_TILE_H
#define SLB_PROJECTS_DEMO_SLIDING_TILE_H

#define SLB_COST_TYPE int
#define SLB_UNIFORM_DOMAIN true
#define SLB_STATE ext::domain::sliding_tile::SlidingTile<>
#define SLB_SLIDING_TILE_NROWS 4
#define SLB_SLIDING_TILE_NCOLUMNS 4
#define SLB_BASE_HEURISTIC ext::domain::sliding_tile::MDHeuristic
#define SLB_BASE_DYNAMIC_HEURISTIC ext::domain::sliding_tile::DynamicMDHeuristic

#endif
