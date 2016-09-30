/// \file
/// \brief Forward declarations of structures containing the node data for
/// search algorithms.
/// \author Meir Goldenberg

#ifndef NODE_KINDS_FWD_H
#define NODE_KINDS_FWD_H

template <class State> struct BaseT;
using Base = BaseT<SLB_STATE>;

template <class State> struct NoDataT;
using NoData = NoDataT<SLB_STATE>;

template <class State> struct ResponsibleGoalT;
using ResponsibleGoal  = ResponsibleGoalT<SLB_STATE>;

#endif
