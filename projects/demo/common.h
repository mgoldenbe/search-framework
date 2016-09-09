#ifndef SLB_CONFIG_COMMON_H
#define SLB_CONFIG_COMMON_H

#define SLB_BUCKET_POSITION_TYPE int
#define SLB_NODE SearchNode<>

#define SLB_OL_KEY_TYPE DefaultOLKeyType
#define SLB_OL_CONTAINER OLMap
#define SLB_OL OpenList

#define SLB_NODE_DATA_T Base
#define SLB_NODE_DATA SLB_NODE_DATA_T<Domains::SLB_STATE>
#define SLB_OL_PRIORITY GreaterPriority_SmallF_LargeG
#define SLB_GOAL_HANDLER SingleGoalHandler
#define SLB_HEURISTIC SimpleHeuristicToGoal_FixedH
#define SLB_ALGORITHM Astar

#endif
