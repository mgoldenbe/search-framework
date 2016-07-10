#ifndef CONFIG_MIN_H
#define CONFIG_MIN_H

#include "common.h"

#define SLB_NODE_DATA NodeBase<SLB_STATE>
#define SLB_OL_PRIORITY GreaterPriority_SmallG

#define SLB_GOAL_HANDLER MultipleGoalHandler

#define SLB_HEURISTIC ZeroHeuristic

#define SLB_ALGORITHM Astar

#endif
