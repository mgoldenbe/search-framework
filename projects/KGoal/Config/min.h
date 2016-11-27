#ifndef SLB_PROJECTS_KGOAL_CONFIG_MIN_H
#define SLB_PROJECTS_KGOAL_CONFIG_MIN_H

#define SLB_NODE_DATA_T ResponsibleGoal

#define SLB_OL_PRIORITY GreaterPriority_SmallF_LargeG

#define SLB_GOAL_HANDLER MinHeuristicGoalHandler

#define SLB_HEURISTIC MinHeuristicToGoals_FixedH
#define SLB_MIN_HEURISTIC_COMPARE std::less

#define SLB_ALGORITHM Astar

#endif
