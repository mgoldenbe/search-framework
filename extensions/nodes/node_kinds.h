#ifndef NODE_KINDS_H
#define NODE_KINDS_H

/// \file
/// \brief Structures containing the node data for several search algorithm
/// variants.
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace node {

/// Regular node storing \c g and \c f.
/// \tparam State_ The state type, represents the domain.
template <class State>
struct BaseT: ManagedNode<> {
    /// The type representing the cost of actions in the domain.
    using CostType = typename State::CostType;

    ///  The default constructor. Initializes the node data with zeros.
    BaseT() : g(0), f(0) {}
    REFLECTABLE((CostType) g, (CostType) f) // These are public for ease of
                                          // access. I don't see any reason
                                          // for making them private and
                                          // having getters and setters.

    /// Returns the heuristic value
    /// \return The heuristic value
    CostType h() const {return f - g;}

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
struct NoDataT: ManagedNode<BaseT<State>> {};

/// Node for multi-goal search. Stores the goal responsible for heuristic value
/// in addition to storing \c g and \c f.
/// \tparam State_ The state type, represents the domain.
template <class State>
struct ResponsibleGoalT : ManagedNode<BaseT<State>> {
    /// The default constructor.
    ResponsibleGoalT() : responsibleGoal(-1) {}
    /// The goal that was responsible for the heuristic value
    REFLECTABLE((int) responsibleGoal)
};

} // namespace
} // namespace
} // namespace

#endif
