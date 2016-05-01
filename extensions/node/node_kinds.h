///@file
///@brief INTERFACES CHECKED.

#ifndef NODE_KINDS_H
#define NODE_KINDS_H

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

template <typename State = SLB_STATE>
struct MyNodeDataT : ManagedNode<NodeBase<State>> {
    MyNodeDataT() : responsibleGoal(0) {}
    REFLECTABLE((State)responsibleGoal) // the goal that was responsible for the
                                        // heuristic value
};

#endif
