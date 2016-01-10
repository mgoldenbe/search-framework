#ifndef FILTER_H
#define FILTER_H

#include <memory>

template <class AlgorithmEvent>
struct FilterBase {
    virtual bool in(const AlgorithmEvent &e) const = 0;
};

template <class AlgorithmEvent>
struct FilterEventType: FilterBase<AlgorithmEvent> {
    FilterEventType() {set(AlgorithmEvent::eventTypeStr);}
    virtual bool in(const AlgorithmEvent &e) const {
        std::string type =
            AlgorithmEvent::eventTypeStr[static_cast<int>(e.type())];
        return ::in(types_, type);
    }
    void set(const std::vector<std::string> &types) { types_ = types; }
    const std::vector<std::string> &get() const { return types_; }

private:
    std::vector<std::string> types_;
};

template <class AlgorithmEvent>
struct FilterState: FilterBase<AlgorithmEvent> {
    using StateSharedPtr = typename AlgorithmEvent::StateSharedPtr;

    virtual bool in(const AlgorithmEvent &e) const {
        if (!states_.size()) return true;
        return ::in(states_, e.state());
    }
    void add(const StateSharedPtr state) { states_.push_back(state); }
    void set(const StateSharedPtr state) {
        states_.clear();
        states_.push_back(state);
    }
    void reset() { states_.clear(); }

private:
    std::vector<StateSharedPtr> states_;
};

template <class AlgorithmEvent>
struct Filter: FilterBase<AlgorithmEvent> {
    Filter() : filters_({&filterEventType_, &filterState_}) {}
    FilterEventType<AlgorithmEvent> &filterEventType() {
        return filterEventType_;
    }
    FilterState<AlgorithmEvent> &filterState() {
        return filterState_;
    }
    virtual bool in(const AlgorithmEvent &e) const override {
        for (auto f : filters_)
            if (!f->in(e)) return false;
        return true;
    }
private:
    FilterEventType<AlgorithmEvent> filterEventType_;
    FilterState<AlgorithmEvent> filterState_;
    std::vector<FilterBase<AlgorithmEvent> *> filters_;
};

#endif
