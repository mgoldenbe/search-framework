#ifndef SLB_CONFIG_PANCAKE_IDASTAR_H
#define SLB_CONFIG_PANCAKE_IDASTAR_H

#include "pancake.astar.h"

#undef SLB_ALGORITHM
#define SLB_ALGORITHM IdAstar

#define SLB_ID_ASTAR_BACKTRACK_LOCK CopyLock

#endif
