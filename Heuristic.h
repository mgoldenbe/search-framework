///@file
///@brief INTERFACES CHECKED.

#ifndef HEURISTIC_H
#define HEURISTIC_H

template <class Instance, class Heuristic>
struct ZeroHeuristic {
    using State = typename Instance::State;
    using CostType = typename State::CostType;

    ZeroHeuristic(Instance &) {}

    template<class Node>
    CostType operator()(Node *n) const {(void)n; return CostType(0);}
};

template <class Instance, class Heuristic>
struct SimpleHeuristicToGoal {
    using State = typename Instance::State;
    using CostType = typename State::CostType;

    SimpleHeuristicToGoal(Instance &instance)
        : goal_(instance.Instance::Goal::state_) {}

    template<class Node>
    CostType operator()(Node *n) const {
        auto &s = n->state();
        return heuristic_(s, goal_);
    }
private:
    State goal_;
    Heuristic heuristic_;
};

template <class Instance, class Heuristic>
struct MinHeuristicToGoals {
    using State = typename Instance::State;
    using CostType = typename State::CostType;

    MinHeuristicToGoals(Instance &instance)
        : goals_(instance.Instance::Goal::states_) {}

    template<class Node>
    CostType operator()(Node *n) const {
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
    // the goals can be changed by a third party, e.g. the GoalHandler
    std::vector<State> &goals_;
    Heuristic heuristic_;
};

#endif
