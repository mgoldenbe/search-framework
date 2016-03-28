#ifndef ALGORITHM_LOGGER_H
#define ALGORITHM_LOGGER_H

#include <unordered_map>

template <class AlgorithmEvent_>
struct AlgorithmLoggerBase {
    using AlgorithmEvent = AlgorithmEvent_;
    using StateSharedPtr = typename AlgorithmEvent::StateSharedPtr;

    ///@name Construction and Assignment
    //@{
    virtual void log(AlgorithmEvent e) {
        const StateSharedPtr &s = e.state();
        stateToLastEventStep_[s] = events_.size();
        auto it = stateToLastEventStep_.find(s);

        e.setStep(events_.size(), -1);
        if (it != stateToLastEventStep_.end())
            e.setStep(events_.size(), it->second);
        events_.push_back(e);
    }
    //@}

    const AlgorithmEvent &event(int step) const { return events_[step]; }

    template <typename charT>
    std::basic_ostream<charT> &dump(std::basic_ostream<charT> &o,
                                    bool dumpLastEvents = false) const {
        o << std::right << std::setfill(' ') << "ALGORITHM LOG"
                  << std::endl;
        o << std::setw(4) << "Num.";
        AlgorithmEvent::dumpTitle(o);

        for (auto el: events_)
            o << el << std::endl;
        if (!dumpLastEvents) return o;
        o << "\nLast events:" << std::endl;
        for (auto el: stateToLastEventStep_)
            o << *(el.first) << ": " << el.second << std::endl;
        return o;
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

template <class AlgorithmEvent_>
using AlgorithmLogger = AlgorithmLoggerBase<AlgorithmEvent_>;

template <class AlgorithmEvent_>
struct NoAlgorithmLogger : AlgorithmLoggerBase<AlgorithmEvent_> {
    using AlgorithmEvent = AlgorithmEvent_;

    virtual void log(AlgorithmEvent) override {}
    template <typename charT>
    std::basic_ostream<charT> &dump(std::basic_ostream<charT> &o,
                                    bool dumpLastEvents = false) const {
        o << "NoLogger!" << std::endl;
        return o;
    }
};

#endif
