#ifndef SLB_PROJECTS_DEMO_INC_WORST_H
#define SLB_PROJECTS_DEMO_INC_WORST_H

#include "common.h"

#define SLB_STATE ext::domain::incWorst::IncWorst
#define SLB_COST_TYPE int
#define SLB_UNIFORM_DOMAIN false

#define SLB_CMD_T_PARAM	class Add = ext::domain::incWorst::CommandLine
#define SLB_CMD_T_ARG Add
#define SLB_CMD_INIT Add(cmd_)

#undef SLB_HEURISTIC
#define SLB_HEURISTIC ext::policy::heuristic::Zero

#endif
