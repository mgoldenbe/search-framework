#ifndef SLB_CONFIG_COMMON_H
#define SLB_CONFIG_COMMON_H

#define SLB_BUCKET_POSITION_TYPE int
#define SLB_NODE slb::core::sb::SearchNode<>

#define SLB_OL_KEY_TYPE ext::policy::openList::DefaultOLKeyType
#define SLB_OL_CONTAINER ext::policy::openList::OLMap
#define SLB_OL ext::policy::openList::BucketedStdMap

#define SLB_NODE_DATA ext::node::Base
#define SLB_OL_PRIORITY ext::policy::openList::GreaterPriority_SmallF_LargeG
#define SLB_GOAL_HANDLER ext::policy::goalHandler::SingleGoal
#define SLB_GENERATOR ext::policy::generator::States
#define SLB_HEURISTIC ext::policy::heuristic::SingleGoal
#define SLB_INIT_HEURISTIC SLB_HEURISTIC
#define SLB_ALGORITHM ext::algorithm::Astar

#define SLB_INSTANCE_MEASURES ext::instanceMeasure::Nothing

#endif
