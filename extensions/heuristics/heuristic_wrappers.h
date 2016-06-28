#ifndef HEURISTIC_WRAPPERS_H
#define HEURISTIC_WRAPPERS_H

/// \file
/// \brief Wrappers around heuristics to be used by the search algorithms.
/// \author Meir Goldenberg

/// Heuristic that always returns zero.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
struct ZeroHeuristic {
    POLICY_TYPES

    /// Initializes the heuristic based on a problem instance.
    ZeroHeuristic(MyAlgorithm &) {}

    /// Computes the heuristic.
    /// \return The heuristic value. In this case, zero.
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
/// \tparam MyAlgorithm The search algorithm.
/// \tparam Heuristic The heuristic type.
template <class MyAlgorithm, class BaseHeuristic = SLB_BASE_HEURISTIC>
struct MinHeuristicToGoals {
    POLICY_TYPES

    /// Initializes the heuristic based on a problem instance.
    /// \param alg Reference to the search algorithm.
    MinHeuristicToGoals(MyAlgorithm &alg) : alg_(alg) {}

    /// Computes the heuristic.
    /// \return The heuristic value.
    CostType operator()(Node *n) const {
        auto &s = n->state();
        auto &goals = alg_.goalHandler().activeGoals();
        int responsible = goals[0].second;
        CostType res = heuristic_(s, goals[0].first);
        // Note that goals.size() can be 0.
        for (auto i = 1U; i < goals.size(); ++i) {
            auto myH = heuristic_(s, goals[i].first);
            if (myH < res) {
                res = myH;
                responsible = goals[i].second;
            }
        }
        n->responsibleGoal = responsible;
        return res;
    }
private:
    MyAlgorithm &alg_;
    BaseHeuristic heuristic_; ///< The underlying heuristic.
};

/// Computes minimum among heuristics to goals that have not been found (with
/// the required quality) yet.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
using MinHeuristicToGoals_FixedH =
    MinHeuristicToGoals<MyAlgorithm, SLB_BASE_HEURISTIC>;

#endif
