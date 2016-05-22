/// \file
/// \brief Structures containing the node data for several search algorithm
/// variants.
/// \author Meir Goldenberg

#ifndef NODE_KINDS_H
#define NODE_KINDS_H

#include "../../core/search_base/managed_node.h"

/// Regular node storing \c g and \c f.
/// \tparam State_ The state type, represents the domain.
template <typename State_>
struct NodeBase: ManagedNode<> {
    using State = State_;
    using CostType = typename State::CostType;
    NodeBase() : g(0), f(0) {}
    REFLECTABLE((CostType) g, (CostType) f)      // These are public for ease of
                                               // access. I don't see any reason
                                               // for making them private and
                                               // having getters and setters.
};

/// Node storing nothing.
/// \tparam State_ The state type, represents the domain.
template <typename State>
struct NoNodeData: ManagedNode<NodeBase<State>> {};

/// Node for multi-goal search. Stores the goal responsible for heuristic value
/// in addition to storing \c g and \c f.
/// \tparam State_ The state type, represents the domain.
template <typename State = SLB_STATE>
struct MyNodeDataT : ManagedNode<NodeBase<State>> {
    MyNodeDataT() : responsibleGoal(0) {}
    REFLECTABLE((State) responsibleGoal) /// the goal that was responsible for
                                        /// the heuristic value
};

#endif
