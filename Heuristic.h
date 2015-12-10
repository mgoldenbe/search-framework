///@file
///@brief INTERFACES CHECKED.

#ifndef HEURISTIC
#define HEURISTIC

template <typename CostType> struct ZeroHeuristic {
    template<class Node>
    CostType operator()(Node *n) const {(void)n; return CostType(0);}
};

template <class State, class Heuristic>
struct MinHeuristicToGoals {
    using CostType = typename State::CostType;

    MinHeuristicToGoals(std::vector<State> &goals, const Heuristic &heuristic)
        : goals_(goals), heuristic_(heuristic) {}

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
        n->heuristicGoal = goals_[responsible];
        return res;
    }
private:
    // the goals can be changed by a third party, e.g. the GoalHandler
    std::vector<State> &goals_;
    const Heuristic &heuristic_;
};

#endif
