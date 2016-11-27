/// \file
/// \brief Forward declarations of structures containing the node data for
/// search algorithms.
/// \author Meir Goldenberg

#ifndef SLB_EXTENSIONS_NODES_NODE_KINDS_FWD_H
#define SLB_EXTENSIONS_NODES_NODE_KINDS_FWD_H

namespace slb {
namespace ext {
namespace node {

template <class State> struct BaseT;
/// Regular node storing \c g and \c f.
using Base = BaseT<SLB_STATE>;

template <class State> struct NoDataT;
/// Node storing nothing.
using NoData = NoDataT<SLB_STATE>;

template <class State> struct ResponsibleGoalT;
/// Node for multi-goal search. Stores the goal responsible for heuristic value
/// in addition to storing \c g and \c f.
using ResponsibleGoal  = ResponsibleGoalT<SLB_STATE>;

} // namespace
} // namespace
} // namespace

#endif
