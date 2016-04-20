///@file
///@brief INTERFACES CHECKED.

#ifndef GOAL_HANDLER_H
#define GOAL_HANDLER_H

template <class State, class Logger> // for uniformity
struct NoGoalHandler {
    using MyInstance = Instance<State>;
    using CostType = typename State::CostType;

    NoGoalHandler(MyInstance &, Logger &) {}
    template <class Node> void onSelect(const Node *, CostType &) {}
    bool done() const {return false;}

    template <class Node> void logInit() {}
};

template <class State, class Logger = LOGGER>
struct SingleGoalHandler {
    using MyInstance = Instance<State>;
    using CostType = typename State::CostType;

    SingleGoalHandler(MyInstance &instance, Logger &logger)
        : goal_(instance.goal()), logger_(logger) {}

    template <class Node> void onSelect(const Node *n, CostType &res) {
        if (n->state() == goal_) {
            // std::cerr << "DONE_GOAL " << n->state() << " " << n->f
            //           << std::endl;
            res = n->f;
            done_ = true;
            log<Events::SolvedGoal>(logger_, n);
            log<Events::DoneSolvedGoal>(logger_, n);
        }
    }
    bool done() const {return done_;}

    template <class Node> void logInit() {
        log<Events::MarkedGoal>(logger_, std::make_shared<Node>(goal_).get());
    }
private:
    State goal_;
    Logger &logger_;
    bool done_ = false;
};

template <class State, class Logger>
struct MultipleGoalHandler {
    using MyInstance = Instance<State>;
    using CostType = typename State::CostType;

    MultipleGoalHandler(MyInstance &instance, Logger &logger)
        : goals_(instance.goals()), logger_(logger) {}

    template <class Node> bool onSelect(const Node *n, CostType &res) {
        { // Check current goals
            auto it = std::find(goals_.begin(), goals_.end(), n->state());
            if (it != goals_.end()) {
                // std::cerr << "DONE_GOAL " << n->state() << " " << n->f
                //           << std::endl;
                res =
                    (res * doneGoals_.size() + n->f) / (doneGoals_.size() + 1);
                goals_.erase(it);
                doneGoals_.push_back(n->state());
                if (goals_.empty()) done_ = true;
                log<Events::SolvedGoal>(logger_, n);
                log<Events::DoneSolvedGoal>(logger_, n);
            }
        }
        return true;
    }
    bool done() const {return done_;}

    template <class Node> void logInit() {
        for (auto &g: goals_)
            log<Events::MarkedGoal>(logger_, std::make_shared<Node>(g).get());
    }
private:
    // changes to goals_ will also be seen by heuristics
    std::vector<State> &goals_;
    std::vector<State> doneGoals_;
    Logger &logger_;
    bool done_ = false;
};

// To be used with MinHeuristicToGoals or some other heuristic that stores the
// goal responsible for the heuristic.
template <class State, class Logger>
struct MinHeuristicGoalHandler {
    using MyInstance = Instance<State>;
    using CostType = typename State::CostType;

    MinHeuristicGoalHandler(MyInstance &instance, Logger &logger)
        : goals_(instance.goals()), logger_(logger) {}

    template <class Node> bool onSelect(const Node *n, CostType &res) {
        { // Check identity of goal resposible for heuristic
            auto it = std::find(doneGoals_.begin(), doneGoals_.end(),
                                n->responsibleGoal);
            if (it != doneGoals_.end()) {
                log<Events::SuspendedExpansion>(logger_, n);
                return false;
            }
        }
        { // Check current goals
            auto it = std::find(goals_.begin(), goals_.end(), n->state());
            if (it != goals_.end()) {
                // std::cerr << "DONE_GOAL " << n->state() << " " << n->f
                //           << std::endl;
                res =
                    (res * doneGoals_.size() + n->f) / (doneGoals_.size() + 1);
                goals_.erase(it);
                doneGoals_.push_back(n->state());
                if (goals_.empty()) done_ = true;
                log<Events::SolvedGoal>(logger_, n);
                log<Events::DoneSolvedGoal>(logger_, n);
            }
        }
        return true;
    }
    bool done() const {return done_;}

    template <class Node> void logInit() {
        for (auto &g : goals_)
            log<Events::MarkedGoal>(logger_, std::make_shared<Node>(g).get());
    }
private:
    // changes to goals_ will also be seen by heuristics
    std::vector<State> &goals_;
    std::vector<State> doneGoals_;
    Logger &logger_;
    bool done_ = false;
};

#endif
