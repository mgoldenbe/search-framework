///@file
///@brief INTERFACES CHECKED.

#ifndef NODE_DATA
#define NODE_DATA

#include "ManagedNode.h"

template <typename State_>
struct NodeBase: ManagedNode<> {
    using State = State_;
    using CostType = typename State::CostType;
    NodeBase() : g(0), f(0) {}
    REFLECTABLE((CostType)g, (CostType)f)      // These are public for ease of
                                               // access. I don't see any reason
                                               // for making them private and
                                               // having getters and setters.
};

template <typename State>
struct NoNodeData: ManagedNode<NodeBase<State>> {};

#endif
