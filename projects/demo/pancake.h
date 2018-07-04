#ifndef SLB_PROJECTS_DEMO_PANCAKE_H
#define SLB_PROJECTS_DEMO_PANCAKE_H

#define SLB_COST_TYPE int
#define SLB_UNIFORM_DOMAIN true
#define SLB_STATE ext::domain::pancake::Pancake

#define SLB_CMD_T_PARAM	class Add = ext::domain::pancake::CommandLine
#define SLB_CMD_T_ARG Add
#define SLB_CMD_INIT Add(cmd_)

#define SLB_BASE_HEURISTIC ext::domain::pancake::GapHeuristic
#define SLB_BASE_DYNAMIC_HEURISTIC ext::domain::pancake::DynamicGapHeuristic

#endif
