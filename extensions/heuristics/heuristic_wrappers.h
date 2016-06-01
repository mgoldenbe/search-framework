#ifndef HEURISTIC_WRAPPERS_H
#define HEURISTIC_WRAPPERS_H

/// \file
/// \brief Wrappers around heuristics to be used by the search algorithms.
/// \author Meir Goldenberg

/// Heuristic that always returns zero.
/// \tparam The state type, represents the domain.
template <class State>
struct ZeroHeuristic {
    /// The problem instance type.
    using MyInstance = Instance<State>;

    /// The type representing the cost of actions in the domain.
    using CostType = typename State::CostType;

    /// Initializes the heuristic based on a problem instance.
    ZeroHeuristic(MyInstance &) {}

    /// Computes the heuristic.
    /// \tparam Node The search node type.
    /// \return The heuristic value. In this case, zero.
    template<class Node>
    CostType operator()(Node *n) const {(void)n; return CostType(0);}
};

/// A simple wrapper around a single heuristic to single goal.
/// \tparam The state type, represents the domain.
/// \tparam Heuristic The heuristic type.
template <class State, class Heuristic>
struct SimpleHeuristicToGoal {
    /// The problem instance type.
    using MyInstance = Instance<State>;

    /// The type representing the cost of actions in the domain.
    using CostType = typename State::CostType;

    /// Initializes the heuristic based on a problem instance.
    SimpleHeuristicToGoal(MyInstance &instance)
        : goal_(instance.goal()) {}

    /// Computes the heuristic.
    /// \tparam Node The search node type.
    /// \return The heuristic value.
    template <class Node> CostType operator()(Node *n) const {
        auto &s = n->state();
        return heuristic_(s, goal_);
    }
private:
    State goal_; ///< The goal state.
    Heuristic heuristic_; ///< The underlying heuristic.
};

/// Computes minimum among heuristics to goals that have not been found (with
/// the required quality) yet.
/// \tparam State The state type, represents the domain.
/// \tparam Heuristic The heuristic type.
template <class State, class Heuristic>
struct MinHeuristicToGoals {
    /// The problem instance type.
    using MyInstance = Instance<State>;

    /// Type for action cost in the search domain.
    using CostType = typename State::CostType;

    /// Initializes the heuristic based on a problem instance.
    MinHeuristicToGoals(MyInstance &instance)
        : goals_(instance.goals()) {}

    /// Computes the heuristic.
    /// \tparam Node The search node type.
    /// \return The heuristic value.
    template <class Node> CostType operator()(Node *n) const {
        auto &s = n->state();
        int responsible = 0;
        CostType res = heuristic_(s, goals_[0]);
        for (auto i = 1U; i < goals_.size(); ++i) {
            auto myH = heuristic_(s, goals_[i]);
            if (myH < res) {
                res = myH;
                responsible = i;
            }
        }
        n->responsibleGoal = goals_[responsible];
        return res;
    }
private:
    /// The goal states.
    /// \note This set of goals can be modified by a third party, e.g. the
    /// GoalHandler.
    std::vector<State> &goals_;

    Heuristic heuristic_; ///< The underlying heuristic.
};

#endif
