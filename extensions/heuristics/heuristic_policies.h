#ifndef HEURISTIC_POLICIES_H
#define HEURISTIC_POLICIES_H

/// \file
/// \brief Wrappers around heuristics to be used by the search algorithms.
/// \author Meir Goldenberg

/// Used by the MinHeuristicToGoals handler.
#define SLB_MIN_HEURISTIC_COMPARE_T                                            \
    SLB_MIN_HEURISTIC_COMPARE<typename AlgorithmTraits<MyAlgorithm>::CostType>

/// Heuristic that always returns zero.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
struct ZeroHeuristic {
    POLICY_TYPES

    /// The constructor.
    ZeroHeuristic(MyAlgorithm &) {}

    /// Computes the heuristic.
    /// \param s The state for which the heuristic is to be computed.
    /// \return The heuristic value. In this case, zero.
    CostType operator()(const State &s, Node *) const { return CostType(0); }
};

/// A simple wrapper around a single heuristic to single goal.
/// \tparam MyAlgorithm The search algorithm.
/// \tparam BaseHeuristic The base heuristic type.
template <class MyAlgorithm, class BaseHeuristic = SLB_BASE_HEURISTIC>
struct SimpleHeuristicToGoal {
    POLICY_TYPES

    /// The constructor.
    SimpleHeuristicToGoal(MyAlgorithm &alg)
        : alg_(alg), goal_(alg.instance().goal()) {}

    /// Computes the heuristic.
    /// \param s The state for which the heuristic is to be computed.
    /// \return The heuristic value.
    CostType operator()(const State &s, Node *) const {
        return heuristic_(s, goal_);
    }

    /// Computes the dynamic heuristic.
    /// \param s The parent state.
    /// \param h The heuristic at \c s.
    /// \param a The action.
    /// \param c The cost of action.
    /// \param g The goal state.
    /// \return The heuristic value.
    int operator()(const State &s, int h, State::Action a,
                   State::CostType c, Node *) const {
        return heuristic_(s, h, a, c, goal_);
    }
private:
    MyAlgorithm &alg_;
    State goal_; ///< The goal state.
    BaseHeuristic heuristic_; ///< The underlying heuristic.
};

/// A simple wrapper around a single heuristic to single goal.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
using SimpleHeuristicToGoal_FixedH =
    SimpleHeuristicToGoal<MyAlgorithm, SLB_BASE_HEURISTIC>;

/// Computes minimum among heuristics to goals that have not been found (with
/// the required quality) yet.
/// \tparam MyAlgorithm The search algorithm.
/// \tparam BaseHeuristic The base heuristic type.
/// \tparam Compare The comparer of heuristic values, e.g. std::less or
/// std::greater.
template <class MyAlgorithm, class BaseHeuristic = SLB_BASE_HEURISTIC,
          class Compare = SLB_MIN_HEURISTIC_COMPARE_T>
struct MinHeuristicToGoals {
    POLICY_TYPES

    /// The constructor.
    /// \param alg Reference to the search algorithm.
    MinHeuristicToGoals(MyAlgorithm &alg) : alg_(alg) {}

    /// Computes the heuristic.
    /// \param s The state for which the heuristic is to be computed.
    /// \param n The node for which the heuristic is to be computed.
    /// \return The heuristic value.
    CostType operator()(const State &s, Node *n) const {
        auto &goals = alg_.goalHandler().activeGoals();
        assert(goals.size() > 0);
        int responsible = goals[0].second;
        CostType res = heuristic_(s, goals[0].first);
        // Note that goals.size() can be 0.
        for (auto i = 1U; i < goals.size(); ++i) {
            auto myH = heuristic_(s, goals[i].first);
            if (Compare{}(myH, res)) {
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
