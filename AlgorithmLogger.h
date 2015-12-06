#ifndef ALGORITHM_LOGGER
#define ALGORITHM_LOGGER

#include <unordered_map>

template <class AlgorithmEvent_>
struct NoAlgorithmLogger {
    using AlgorithmEvent = AlgorithmEvent_;
    using StateSharedPtr = typename AlgorithmEvent::StateSharedPtr;

    void log(const AlgorithmEvent e) {
        (void)e;
    }
    void dump() { std::cout << "NoLogger!" << std::endl; }
};

template <class AlgorithmEvent_>
struct AlgorithmLogger {
    using AlgorithmEvent = AlgorithmEvent_;
    using StateSharedPtr = typename AlgorithmEvent::StateSharedPtr;

    void log(AlgorithmEvent e) {
        const StateSharedPtr &s = e.state();
        stateToLastEventStep_[s] = events_.size();
        auto it = stateToLastEventStep_.find(s);

        e.setLastEventStep(-1);
        if (it != stateToLastEventStep_.end())
            e.setLastEventStep(it->second);
        events_.push_back(e);
    }
    void dump() {
        std::cout << "Events in order:" << std::endl;
        int i = 0;
        for (auto el: events_)
            std::cout << "Event " << i++ << ". " << el << std::endl;
        std::cout << "\nLast events:" << std::endl;
        for (auto el: stateToLastEventStep_)
            std::cout << *(el.first) << ": " << el.second << std::endl;
    }

    const std::vector<AlgorithmEvent> &events() const {return events_;}

    const AlgorithmEvent &getLastEvent(const StateSharedPtr &s) const {
        auto it = stateToLastEventStep_.find(s);
        if (it == stateToLastEventStep_.end()) assert(0);
        return events_[it->second];
    }
private:
    // -1 means no previous event
    std::unordered_map<StateSharedPtr, int,
                       StateSharedPtrHash<typename AlgorithmEvent::State>>
    stateToLastEventStep_;

    std::vector<AlgorithmEvent> events_;
};

#endif
