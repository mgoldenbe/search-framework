#ifndef SLB_CONFIG_PANCAKE_IDASTAR_H
#define SLB_CONFIG_PANCAKE_IDASTAR_H

#include "pancake.astar.dynamic.h"

#undef SLB_ALGORITHM
#define SLB_ALGORITHM ext::algorithm::IdAstar

#define SLB_ID_ASTAR_BACKTRACK_LOCK ext::policy::backtrackLock::Inplace

#endif
