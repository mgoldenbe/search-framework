#ifndef SLB_PROJECTS_DEMO_INC_WORST_H
#define SLB_PROJECTS_DEMO_INC_WORST_H

#include "common.h"

#define SLB_STATE ext::domain::incWorst::IncWorst
#define SLB_COST_TYPE int
#define SLB_UNIFORM_DOMAIN false
#define SLB_CMD_LINE_ADD ext::domain::incWorst::CommandLine

#undef SLB_HEURISTIC
#define SLB_HEURISTIC ext::policy::heuristic::Zero

#endif
