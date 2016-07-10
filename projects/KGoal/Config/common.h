#ifndef CONFIG_COMMON_H
#define CONFIG_COMMON_H

#define SLB_STATE GridMapState<>
#define SLB_COST_TYPE double
#define SLB_UNIFORM_DOMAIN false
#define SLB_COST_DIAGONAL 1.5

#define SLB_STATE_SMART_PTR StateSharedPtrT
#define SLB_BUCKET_POSITION_TYPE int
#define SLB_NODE SearchNode<>

#define SLB_OL_KEY_TYPE DefaultOLKeyType
#define SLB_OL_CONTAINER OLMap
#define SLB_OL OpenList

#define SLB_INSTANCE_MEASURES InstanceMeasures::CostGoal0Goal1

#endif
