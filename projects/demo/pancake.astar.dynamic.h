#ifndef SLB_CONFIG_PANCAKE_DYNAMIC_H
#define SLB_CONFIG_PANCAKE_DYNAMIC_H

#include "common.h"

#define SLB_DOMAINS_PANCAKE
#define SLB_COST_TYPE int
#define SLB_UNIFORM_DOMAIN true
#define SLB_STATE Pancake
#define SLB_CMD_LINE_ADD CommandLine::Pancake
#define SLB_BASE_HEURISTIC Domains::GapHeuristic

#define SLB_BASE_DYNAMIC_HEURISTIC Domains::DynamicGapHeuristic

#undef SLB_GENERATOR
#define SLB_GENERATOR ActionsGeneratorPolicy

#undef SLB_HEURISTIC
#define SLB_HEURISTIC HeuristicPolicy::DynamicSingleGoal

#undef SLB_INIT_HEURISTIC
#define SLB_INIT_HEURISTIC HeuristicPolicy::SingleGoal
#endif
