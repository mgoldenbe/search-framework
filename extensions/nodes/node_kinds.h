/// \file
/// \brief Structures containing the node data for several search algorithm
/// variants.
/// \author Meir Goldenberg

#ifndef NODE_KINDS_H
#define NODE_KINDS_H

#include "../../core/search_base/managed_node.h"

/// Regular node storing \c g and \c f.
/// \tparam State_ The state type, represents the domain.
template <typename State>
struct NodeBase: ManagedNode<> {
    /// The type representing the cost of actions in the domain.
    using CostType = typename State::CostType;

    /// Updates g-value of the node
    /// \param newG The new g-value.
    void updateG(CostType newG) {
        f += (newG - g);
        g = newG;
    }

    /// Updates h-value of the node
    /// \param newH The new h-value.
    void updateH(CostType newH) {
        f = g + newH;
    }

    ///  The default constructor. Initializes the node data with zeros.
    NodeBase() : g(0), f(0) {}
    REFLECTABLE((CostType) g, (CostType) f)      // These are public for ease of
                                               // access. I don't see any reason
                                               // for making them private and
                                               // having getters and setters.
};

/// Node storing nothing.
/// \tparam State The state type, represents the domain.
template <typename State>
struct NoNodeData: ManagedNode<NodeBase<State>> {};

/// Node for multi-goal search. Stores the goal responsible for heuristic value
/// in addition to storing \c g and \c f.
/// \tparam State_ The state type, represents the domain.
template <typename State = SLB_STATE>
struct NodeWithResponsibleGoal : ManagedNode<NodeBase<State>> {
    /// The default constructor. Initializes the node data with zeros and the
    /// first goal being the responsible one.
    NodeWithResponsibleGoal() : responsibleGoal(-1) {}
    REFLECTABLE((int) responsibleGoal) /// the goal that was responsible for
                                        /// the heuristic value
};

#endif
