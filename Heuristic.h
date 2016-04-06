///@file
///@brief INTERFACES CHECKED.

#ifndef HEURISTIC_H
#define HEURISTIC_H

template <class State, class Heuristic>
struct ZeroHeuristic {
    using MyInstance = Instance<State>;
    using CostType = typename State::CostType;

    ZeroHeuristic(MyInstance &) {}

    template<class Node>
    CostType operator()(Node *n) const {(void)n; return CostType(0);}
};

template <class State, class Heuristic>
struct SimpleHeuristicToGoal {
    using MyInstance = Instance<State>;
    using CostType = typename State::CostType;

    SimpleHeuristicToGoal(MyInstance &instance)
        : goal_(instance.goal()) {}

    template<class Node>
    CostType operator()(Node *n) const {
        auto &s = n->state();
        return heuristic_(s, goal_);
    }
private:
    State goal_;
    Heuristic heuristic_;
};

template <class State, class Heuristic>
struct MinHeuristicToGoals {
    using MyInstance = Instance<State>;
    using CostType = typename State::CostType;

    MinHeuristicToGoals(MyInstance &instance)
        : goals_(instance.goals()) {}

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
