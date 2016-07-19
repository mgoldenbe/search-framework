/// \file
/// \brief Structures containing the node data for several search algorithm
/// variants.
/// \author Meir Goldenberg

#ifndef NODE_KINDS_H
#define NODE_KINDS_H

namespace Node {

/// Regular node storing \c g and \c f.
/// \tparam State_ The state type, represents the domain.
template <class State>
struct Base: ManagedNode<> {
    /// The type representing the cost of actions in the domain.
    using CostType = typename State::CostType;

    ///  The default constructor. Initializes the node data with zeros.
    Base() : g(0), f(0) {}
    REFLECTABLE((CostType) g, (CostType) f)      // These are public for ease of
                                               // access. I don't see any reason
                                               // for making them private and
                                               // having getters and setters.

    /// Updates g-value of the node
    /// \param newG The new g-value.
    void updateG(CostType newG) {
        f += (newG - g);
        g = newG;
    }

    /// Updates h-value of the node
    /// \param newG The new g-value.
    void updateH(CostType newH) {
        f = g + newH;
    }

    /// Sets g and f of the node.
    /// \tparam Node The search node type.
    /// \param g The g-value.
    /// \param h The h-value.
    void set(CostType g, CostType h, int) {
        this->g = g;
        this->f = g + h;
    }
};

/// Node storing nothing.
/// \tparam State The state type, represents the domain.
template <class State>
struct NoData: ManagedNode<Base<State>> {};

/// Node for multi-goal search. Stores the goal responsible for heuristic value
/// in addition to storing \c g and \c f.
/// \tparam State_ The state type, represents the domain.
template <class State>
struct ResponsibleGoal : ManagedNode<Base<State>> {
    /// The default constructor.
    ResponsibleGoal() : responsibleGoal(-1) {}
    /// The goal that was responsible for the heuristic value
    REFLECTABLE((int) responsibleGoal)
};

}
#endif
