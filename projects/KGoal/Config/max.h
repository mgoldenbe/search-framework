#ifndef SLB_PROJECTS_KGOAL_CONFIG_MAX_H
#define SLB_PROJECTS_KGOAL_CONFIG_MAX_H

#include "min.h"

#undef SLB_GOAL_HANDLER
#define SLB_GOAL_HANDLER MaxHeuristicGoalHandler

#undef SLB_MIN_HEURISTIC_COMPARE
#define SLB_MIN_HEURISTIC_COMPARE std::greater

#endif
