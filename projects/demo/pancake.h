#ifndef SLB_CONFIG_PANCAKE_H
#define SLB_CONFIG_PANCAKE_H

#define SLB_DOMAINS_PANCAKE
#define SLB_COST_TYPE int
#define SLB_UNIFORM_DOMAIN true
#define SLB_STATE ext::domain::pancake::Pancake
#define SLB_CMD_LINE_ADD ext::domain::pancake::CommandLine
#define SLB_BASE_HEURISTIC ext::domain::pancake::GapHeuristic

#endif
