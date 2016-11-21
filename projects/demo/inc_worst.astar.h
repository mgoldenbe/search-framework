#ifndef SLB_CONFIG_INC_WORST_ASTAR_H
#define SLB_CONFIG_INC_WORST_ASTAR_H

#include "common.h"
#include "neighbors.h"
#include "astar.h"
#include "inc_worst.h"

#undef SLB_OL_PRIORITY
#define SLB_OL_PRIORITY GreaterPriority_SmallG

#endif
