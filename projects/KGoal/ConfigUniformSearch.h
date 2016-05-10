#ifndef CONFIG_MIN_HEURISTIC_H
#define CONFIG_MIN_HEURISTIC_H

#define SLB_STATE GridMapState<>
#define SLB_COST_TYPE double
#define SLB_UNIFORM_DOMAIN false
#define SLB_NODE_DATA MyNodeDataT<SLB_STATE>

#define SLB_STATE_SMART_PTR StateSharedPtrT
#define SLB_BUCKET_POSITION_TYPE int
#define SLB_NODE AStarNode<>

// Can't put defaults in OL, since there are several instantiations
#define SLB_OL_PRIORITY_TYPE DefaultPriority
#define SLB_OL_PRIORITY GreaterPriority_SmallF_LargeG
#define SLB_OL_CONTAINER OLMap
#define SLB_OL OpenList<>

#define SLB_GOAL_HANDLER MultipleGoalHandler

#define SLB_HEURISTIC ZeroHeuristic<SLB_STATE>
#define SLB_ALGORITHM Astar
#define SLB_RAW_ALGORITHM SLB_ALGORITHM

#endif
