#ifndef SLB_CONFIG_PANCAKE_DYNAMIC_H
#define SLB_CONFIG_PANCAKE_DYNAMIC_H

#include "pancake.astar.h"

#define SLB_BASE_DYNAMIC_HEURISTIC ext::domain::pancake::DynamicGapHeuristic

#undef SLB_GENERATOR
#define SLB_GENERATOR ext::policy::generator::Actions

#undef SLB_HEURISTIC
#define SLB_HEURISTIC ext::policy::heuristic::DynamicSingleGoal

#undef SLB_INIT_HEURISTIC
#define SLB_INIT_HEURISTIC ext::policy::heuristic::SingleGoal
#endif
