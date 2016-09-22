#ifndef HEURISTIC_POLICIES_H
#define HEURISTIC_POLICIES_H

/// \file
/// \brief Wrappers around heuristics to be used by the search algorithms.
/// \author Meir Goldenberg

/// Used by the MinHeuristicToGoals handler.
#define SLB_MIN_HEURISTIC_COMPARE_T                                            \
    SLB_MIN_HEURISTIC_COMPARE<typename AlgorithmTraits<MyAlgorithm>::CostType>

namespace HeuristicPolicy {

/// Heuristic that always returns zero.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
struct Zero {
    POLICY_TYPES

    /// The constructor.
    Zero(MyAlgorithm &) {}

    /// Computes the heuristic (zero in this case).
    /// \tparam Neighbor The neighbor type.
    /// \return The heuristic value. In this case, zero.
    template <class Neighbor>
    CostType operator()(const Neighbor &, Node *) const { return CostType(0); }

    /// Computes the heuristic (zero in this case).
    /// \return The heuristic value. In this case, zero.
    CostType operator()(Node *) const { return CostType(0); }
};

/// Heuristic to single goal.
/// \tparam MyAlgorithm The search algorithm.
/// \tparam BaseHeuristic The base heuristic type.
template <class MyAlgorithm, class BaseHeuristic>
struct SingleGoalT {
    POLICY_TYPES

    /// The constructor. The goal state is taken from the algorithm by using the policy service.
    /// \param alg Reference to the algorithm
    SingleGoalT(MyAlgorithm &alg) : alg_{alg}, heuristic_{alg_.instance().goal()} {}

    /// The constructor with the goal state supplied as an argument.
    /// \param alg Reference to the algorithm
    /// \param goal The goal state.
    SingleGoalT(MyAlgorithm &alg, const State &goal) : alg_{alg}, heuristic_{goal} {}

    /// Computes the heuristic.
    /// \tparam Neighbor The neighbor type.
    /// \param node The search node for which the heuristic is to be computed.
    /// \return The heuristic value.
    template <class Neighbor>
    CostType operator()(const Neighbor &, Node *node) const {
        return operator()(node);
    }

    /// Computes the heuristic.
    /// \param node The search node for which the heuristic is to be computed.
    /// \return The heuristic value.
    CostType operator()(Node *node) const {
        return heuristic_(node->state());
    }

private:
    MyAlgorithm &alg_; ///< The algorithm reference.
    BaseHeuristic heuristic_; ///< The underlying heuristic.
};
/// The policy of heuristic to a single goal.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
using SingleGoal = SingleGoalT<MyAlgorithm, SLB_BASE_HEURISTIC>;

/// Dynamic heuristic to single goal.
/// \tparam MyAlgorithm The search algorithm.
/// \tparam BaseHeuristic The base heuristic type.
template <class MyAlgorithm, class BaseHeuristic>
struct DynamicSingleGoalT {
    POLICY_TYPES

    /// The constructor. The goal state is taken from the algorithm by using the policy service.
    /// \param alg Reference to the algorithm
    DynamicSingleGoalT(MyAlgorithm &alg) : alg_{alg}, heuristic_{alg_.instance().goal()} {}

    /// The constructor with the goal state supplied as an argument.
    /// \param alg Reference to the algorithm
    /// \param goal The goal state.
    DynamicSingleGoalT(MyAlgorithm &alg, const State &goal) : alg_{alg}, heuristic_{goal} {}

    /// Computes the heuristic.
    /// \tparam Neighbor The neighbor type.
    /// \param n The neighbor for which the heuristic is to be computed.
    /// \param node The search node.
    /// \return The heuristic value.
    template <class Neighbor>
    CostType operator()(const Neighbor &n, Node *node) const {
        assert(node->parent());
        return node->parent()->h() + heuristic_(node->parent()->state(), n.cost(), n);
    }

private:
    MyAlgorithm &alg_; ///< The algorithm reference.
    BaseHeuristic heuristic_; ///< The underlying heuristic.
};
/// The policy of dynamic heuristic to default goal.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
using DynamicSingleGoal = DynamicSingleGoalT<MyAlgorithm, SLB_BASE_DYNAMIC_HEURISTIC>;

/// Computes minimum among heuristics to goals that have not been found (with
/// the required quality) yet.
/// \tparam MyAlgorithm The search algorithm.
/// \tparam SingleGoalHeuristic The heuristic for a single goal.
/// \tparam Compare The comparer of heuristic values, e.g. std::less or
/// std::greater.
template <class MyAlgorithm, class SingleGoalHeuristic, class Compare>
struct MinMultipleGoalsT {
    POLICY_TYPES

    /// The constructor.
    /// \param alg Reference to the search algorithm.
    MinMultipleGoalsT(MyAlgorithm &alg) : alg_(alg) {
        for (const auto &el: alg_.instance().goals())
            singleGoalHeuristics_.push_back(alg, SingleGoalHeuristic{el});
    }

    /// Computes the heuristic.
    /// \tparam Neighbor The neighbor type.
    /// \param n The state for which the heuristic is to be computed.
    /// \param node The node for which the heuristic is to be computed.
    /// \return The heuristic value.
    template <class Neighbor>
    CostType operator()(const Neighbor &n, Node *node) const {
        return compute([this, &n, node](
                           int i) { return singleGoalHeuristics_[i](n, node); },
                       node);
    }

    /// Computes the heuristic.
    /// \param node The node for which the heuristic is to be computed.
    /// \return The heuristic value.
    CostType operator()(Node *node) const {
        return compute(
            [this, node](int i) { return singleGoalHeuristics_[i](node); },
            node);
    }
private:
    MyAlgorithm &alg_;
    std::vector<SingleGoalHeuristic>
        singleGoalHeuristics_; ///< The single goal heuristic.

    /// Actual computation of the heuristic.
    /// \tparam L The lambda type.
    /// \param singleH A lambda for calling the single-goal heuristic with
    /// correct arguments.
    /// \param node The search node.
    /// \return The heuristic value.
    template <class L>
    CostType compute(L singleH, Node *node) const {
        auto &goals = alg_.goalHandler().activeGoals();
        assert(goals.size() > 0);
        int curActiveIndex = goals[0].second;
        int responsible = curActiveIndex;
        CostType res = singleH(curActiveIndex);
        // Note that goals.size() can be 0.
        for (auto i = 1U; i < goals.size(); ++i) {
            curActiveIndex = goals[i].second;
            auto myH = singleH(curActiveIndex);
            if (Compare{}(myH, res)) {
                res = myH;
                responsible = goals[i].second;
            }
        }
        node->responsibleGoal = responsible;
        return res;
    }
};
/// Computes minimum among heuristics to goals that have not been found (with
/// the required quality) yet.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
using MinMultipleGoals =
    MinMultipleGoalsT<MyAlgorithm, SLB_SINGLE_GOAL_HEURISTIC,
                         SLB_MIN_HEURISTIC_COMPARE_T>;
}

#endif
