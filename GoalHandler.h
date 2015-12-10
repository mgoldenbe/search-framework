///@file
///@brief INTERFACES CHECKED.

#ifndef GOAL_HANDLER
#define GOAL_HANDLER

struct NoGoalHandler {
    template <class Node> void onSelect(const Node *n) { (void)n; }
    bool done() const {return false;}

    template <class Node> void logInit() {}
};

template <class State, class Logger>
struct SingleGoalHandler {
    SingleGoalHandler(const State &goal, Logger &logger)
        : goal_(goal), logger_(logger) {}
    template <class Node> void onSelect(const Node *n) {
        using Event = typename Logger::AlgorithmEvent;
        if (n->state() == goal_) {
            done_ = true;
            logger_.log(Event(n->shareState(), Event::StateRole::DONE_GOAL,
                              n->shareParentState()));
        }
    }
    bool done() const {return done_;}

    template <class Node> void logInit() {
        using Event = typename Logger::AlgorithmEvent;
        logger_.log(
            Event(std::make_shared<Node>(goal_), Event::StateRole::GOAL));
    }
private:
    State goal_;
    Logger &logger_;
    bool done_ = false;
};

// To be used with MinHeuristicToGoals or some other heuristic that stores the
// goal responsible for the heuristic.
template <class State, class Logger>
struct MultipleGoalHandler {
    MultipleGoalHandler(std::vector<State> &goals, Logger &logger)
        : goals_(goals), logger_(logger) {}
    template <class Node> bool onSelect(const Node *n) {
        using Event = typename Logger::AlgorithmEvent;
        { // Check identity of goal resposible for heuristic
            auto it = std::find(doneGoals_.begin(), doneGoals_.end(),
                                n->heuristicGoal);
            if (it != doneGoals_.end()) {
                logger_.log(Event(n, Event::EventType::SUSPENDED_EXPANSION));
                return false;
            }
        }
        { // Check current goals
            auto it = std::find(goals_.begin(), goals_.end(), n->state());
            if (it != goals_.end()) {
                goals_.erase(it);
                doneGoals_.push_back(n->state());
                if (goals_.empty()) done_ = true;
                logger_.log(Event(n, Event::StateRole::DONE_GOAL));
            }
        }
        return true;
    }
    bool done() const {return done_;}

    template <class Node> void logInit() {
        using Event = typename Logger::AlgorithmEvent;
        for (auto &g: goals_)
            logger_.log(
                Event(std::make_shared<Node>(g).get(), Event::StateRole::GOAL));
    }
private:
    // changes to goals_ will also be seen by heuristics
    std::vector<State> &goals_;
    std::vector<State> doneGoals_;
    Logger &logger_;
    bool done_ = false;
};

#endif
