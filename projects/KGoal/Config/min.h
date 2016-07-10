#ifndef CONFIG_MIN_H
#define CONFIG_MIN_H

#include "common.h"

#define SLB_NODE_DATA NodeWithResponsibleGoal<SLB_STATE>
#define SLB_OL_PRIORITY GreaterPriority_SmallF_LargeG

#define SLB_GOAL_HANDLER MinHeuristicGoalHandler

#define SLB_BASE_HEURISTIC OctileHeuristic
#define SLB_HEURISTIC MinHeuristicToGoals_FixedH

#define SLB_MIN_HEURISTIC_COMPARE std::less
#define SLB_ALGORITHM Astar

#endif
