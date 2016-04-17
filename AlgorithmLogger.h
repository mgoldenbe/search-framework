#ifndef ALGORITHM_LOGGER_H
#define ALGORITHM_LOGGER_H

namespace Events {
template <class Node> struct Base;
}

template <class Node>
struct AlgorithmLog {
    using Event = std::shared_ptr<Events::Base<Node>>;
    using State = typename Node::State;
    using StateSharedPtr = std::shared_ptr<const State>;

    void log(const StateSharedPtr &s, Event e) {
        stateToLastEventStep_[s] = events_.size();
        events_.push_back(e);
        push_back_unique(eventStrings_, e->eventStr());
    }

    int size() const { return events_.size(); }

    const Event event(int step) const { return events_[step]; }

    template <typename charT>
    std::basic_ostream<charT> &dump(std::basic_ostream<charT> &o,
                                    bool dumpLastEvents = false) const {
        o << std::right << std::setfill(' ') << "ALGORITHM LOG"
                  << std::endl;
        o << std::setw(4) << "Num.";
        Events::Base<Node>::dumpTitle(o);

        for (auto el: events_)
            o << *el << std::endl;
        if (!dumpLastEvents) return o;
        o << "\nLast events:" << std::endl;
        for (auto el: stateToLastEventStep_)
            o << *(el.first) << ": " << el.second << std::endl;
        return o;
    }

    const std::vector<Event> &events() const {return events_;}

    const std::vector<std::string> &eventStrings() const {return eventStrings_;}

    const Event getLastEvent(const StateSharedPtr &s,
                             bool throwFlag = true) const {
        auto it = stateToLastEventStep_.find(s);
        if (it == stateToLastEventStep_.end()) {
            if (!throwFlag) return nullptr;
            throw std::runtime_error("Could not find last event for " + str(*s));
        }
        return events_[it->second];
    }
private:
    // -1 means no previous event
    std::unordered_map<StateSharedPtr, int,
                       StateSharedPtrHash<State>>
    stateToLastEventStep_;

    std::vector<Event> events_;
    std::vector<std::string> eventStrings_;
};

template <template <class> class Event, class Logger, class Node>
void rawLog(Logger &, const Node *, std::false_type) {}
template <template <class> class Event, class Logger, class Node>
void rawLog(Logger &logger, const Node *n, std::true_type) {
    logger.log(n->shareState(),
               std::make_shared<Event<Node>>(logger, n));
}
template <template <class> class Event, class Logger, class Node>
void log(Logger &logger, const Node *n) {
    constexpr bool loggerFlag = !std::is_same<Logger, Nothing>::value;
    return rawLog<Event>(logger, n, std::integral_constant<bool, loggerFlag>{});
}

#endif
