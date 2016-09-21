#ifndef SLB_CONFIG_PANCAKE_H
#define SLB_CONFIG_PANCAKE_H

#include "common.h"

#define SLB_DOMAINS_PANCAKE
#define SLB_COST_TYPE int
#define SLB_UNIFORM_DOMAIN true
#define SLB_STATE Pancake
#define SLB_CMD_LINE_ADD CommandLine::Pancake
#define SLB_BASE_HEURISTIC Domains::GapHeuristicToGoal
#define SLB_INIT_HEURISTIC SLB_HEURISTIC
#define SLB_GENERATOR StatesGeneratorPolicy

#endif
