#ifndef CONFIG_MAX_H
#define CONFIG_MAX_H

#include "min.h"

#undef SLB_NODE_DATA
#define SLB_NODE_DATA NodeWithResponsibleGoal<SLB_STATE>

#undef SLB_GOAL_HANDLER
#define SLB_GOAL_HANDLER MaxHeuristicGoalHandler

#undef SLB_MIN_HEURISTIC_COMPARE
#define SLB_MIN_HEURISTIC_COMPARE std::greater

#endif
