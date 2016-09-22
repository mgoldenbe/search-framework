#ifndef SLB_CONFIG_INC_WORST_H
#define SLB_CONFIG_INC_WORST_H

#include "common.h"

#define SLB_DOMAINS_INC_WORST
#define SLB_STATE IncWorst
#define SLB_COST_TYPE int
#define SLB_UNIFORM_DOMAIN false
#define SLB_CMD_LINE_ADD CommandLine::IncWorst

#undef SLB_HEURISTIC
#define SLB_HEURISTIC HeuristicPolicy::Zero

#undef SLB_OL_PRIORITY
#define SLB_OL_PRIORITY GreaterPriority_SmallG

#endif
