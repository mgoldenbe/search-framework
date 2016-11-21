#ifndef SLB_CONFIG_ASTAR_H
#define SLB_CONFIG_ASTAR_H

#define SLB_ALGORITHM ext::algorithm::Astar
#define SLB_OL_KEY_TYPE ext::policy::openList::DefaultOLKeyType
#define SLB_OL_CONTAINER ext::policy::openList::OLMap
#define SLB_OL ext::policy::openList::BucketedStdMap
#define SLB_OL_PRIORITY ext::policy::openList::GreaterPriority_SmallF_LargeG

#endif
