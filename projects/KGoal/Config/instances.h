#ifndef CONFIG_MIN_H
#define CONFIG_MIN_H

#include "common.h"

#define SLB_NODE_DATA NodeWithResponsibleGoal<SLB_STATE>
#define SLB_OL_PRIORITY GreaterPriority_SmallF_LargeG

//#define SLB_GOAL_HANDLER MinHeuristicGoalHandler
#define SLB_GOAL_HANDLER MaxHeuristicGoalHandler

#define SLB_BASE_HEURISTIC OctileHeuristic
#define SLB_HEURISTIC MinHeuristicToGoals_FixedH

//#define SLB_MIN_HEURISTIC_COMPARE std::less
#define SLB_MIN_HEURISTIC_COMPARE std::greater
#define SLB_ALGORITHM Astar
//#define SLB_RAW_ALGORITHM SLB_ALGORITHM

#define SLB_INSTANCE_MEASURES InstanceMeasures::CostGoal0Goal1

#endif
