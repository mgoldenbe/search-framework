#ifndef GOAL_HANDLER
#define GOAL_HANDLER

template <class State>
struct NoGoalHandler {
    NoGoalHandler(const State &goal) {(void)goal;}
    void update(const State &s) {(void)s;}
    bool done() const {return false;}
    template <class Logger> void logInit(Logger &logger) { (void)logger; }
};

template <class State>
struct SingleGoalHandler {
    SingleGoalHandler(const State &goal) : goal_(goal) {}
    template<class Node, class Logger>
    void update(const Node *n, Logger &logger) {
        using Event = typename Logger::AlgorithmEvent;
        if (n->state() == goal_) {
            done_ = true;
            logger.log(Event(n->shareState(), Event::StateRole::DONE_GOAL,
                             n->shareParentState()));
        }
    }
    bool done() const {return done_;}
    template<class Logger>
    void logInit(Logger &logger) {
        using Event = typename Logger::AlgorithmEvent;
        logger.log(Event(std::make_shared<State>(goal_), Event::StateRole::GOAL,
                         nullptr));
    }
private:
    State goal_;
    bool done_ = false;
};

#endif
