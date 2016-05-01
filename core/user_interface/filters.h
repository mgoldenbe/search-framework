#ifndef FILTER_H
#define FILTER_H

template <class Node>
struct FilterBase {
    using AlgorithmEvent = typename Events::Base<Node>::Event;
    virtual bool in(const AlgorithmEvent &e) const = 0;
};

template <class Node>
struct FilterEventType: FilterBase<Node> {
    using AlgorithmEvent = typename Events::Base<Node>::Event;
    using StateSharedPtr = typename Events::Base<Node>::StateSharedPtr;
    FilterEventType(const std::vector<std::string> &types) : types_(types) {}
    virtual bool in(const AlgorithmEvent &e) const {
        return ::in(types_, e->eventStr());
    }
    void set(const std::vector<std::string> &types) { types_ = types; }
    const std::vector<std::string> &get() const { return types_; }

private:
    std::vector<std::string> types_;
};

template <class Node>
struct FilterState: FilterBase<Node> {
    using AlgorithmEvent = typename Events::Base<Node>::Event;
    using StateSharedPtr = typename Events::Base<Node>::StateSharedPtr;

    virtual bool in(const AlgorithmEvent &e) const {
        if (!states_.size()) return true;
        return ::in(states_, e->state());
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

template <class Node>
struct Filter: FilterBase<Node> {
    using AlgorithmEvent = typename Events::Base<Node>::Event;
    using StateSharedPtr = typename Events::Base<Node>::StateSharedPtr;

    Filter(const std::vector<std::string> &types)
        : filterEventType_(types),
          filters_({&filterEventType_, &filterState_}) {}
    FilterEventType<Node> &filterEventType() {
        return filterEventType_;
    }
    FilterState<Node> &filterState() {
        return filterState_;
    }
    virtual bool in(const AlgorithmEvent &e) const override {
        for (auto f : filters_)
            if (!f->in(e)) return false;
        return true;
    }
private:
    FilterEventType<Node> filterEventType_;
    FilterState<Node> filterState_;
    std::vector<FilterBase<Node> *> filters_;
};

#endif
