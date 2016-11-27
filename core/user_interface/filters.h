#ifndef SLB_CORE_USER_INTERFACE_FILTERS_H
#define SLB_CORE_USER_INTERFACE_FILTERS_H

/// \file
/// \brief Event filters for log analysis and visualization.
/// \author Meir Goldenberg

namespace slb {
namespace core {
namespace ui {

/// The abstract base for all filters.
/// \tparam Node The search node type. It is needed since some filters might
/// operate on the information stored with the node.
template <class Node>
struct FilterBase {
    /// The type representing an event generated by the search algorithm.
    using Event = typename EventBase<Node>::Event;

    /// Smart pointer to state type representing the domain.
    using StateSharedPtr = typename EventBase<Node>::StateSharedPtr;

    /// Determines whether the given event is not filtered out.
    /// \param e The event of interest.
    /// \return \c true if the given event is not filtered out.
    virtual bool in(const Event &e) const = 0;
};

/// Filters based on description strings of events.
/// \tparam Node The search node type.
template <class Node> struct FilterEventStr : FilterBase<Node> {
    /// The type representing an event generated by the search algorithm.
    using Event = typename FilterBase<Node>::Event;

    /// Smart pointer to state type representing the domain.
    using StateSharedPtr = typename FilterBase<Node>::StateSharedPtr;

    /// Initializes the filter based on the description strings of events that
    /// should not be filtered out.
    /// \param strings The description strings of events that
    /// should not be filtered out.
    FilterEventStr(const std::vector<std::string> &strings)
        : strings_(strings) {}

    /// Determines whether the given event is not filtered out.
    /// \param e The event of interest.
    /// \return \c true if the given event is not filtered out.
    virtual bool in(const Event &e) const {
        return util::in(strings_, e->eventStr());
    }

    /// Sets the description strings of events that
    /// should not be filtered out.
    /// \param strings The description strings of events that
    /// should not be filtered out.
    void set(const std::vector<std::string> &strings) { strings_ = strings; }

    /// Returns the description strings of events that
    /// should not be filtered out.
    /// \return The vector of description strings of events that
    /// should not be filtered out.
    const std::vector<std::string> &get() const { return strings_; }

private:
    /// The vector of description strings of events that
    /// should not be filtered out.
    std::vector<std::string> strings_;
};

/// Filters by leaving only events related to chosen states.
/// When the list of states is empty, no events are filtered out.
/// \tparam Node The search node type.
template <class Node>
struct FilterState: FilterBase<Node> {
    /// The type representing an event generated by the search algorithm.
    using Event = typename FilterBase<Node>::Event;

    /// Smart pointer to state type representing the domain.
    using StateSharedPtr = typename FilterBase<Node>::StateSharedPtr;

    /// Determines whether the given event is not filtered out.
    /// \param e The event of interest.
    /// \return \c true if the given event is not filtered out.
    virtual bool in(const Event &e) const {
        if (!states_.size()) return true;
        return util::in(states_, e->state());
    }

    /// Adds a state to the list of the states of interest.
    /// \param state Smart pointer to the state of interest.
    void add(const StateSharedPtr state) { states_.push_back(state); }

    /// Resets the list of the states of interest to contain only the given
    /// state.
    /// \param state Smart pointer to the state of interest.
    void set(const StateSharedPtr state) {
        reset();
        states_.push_back(state);
    }

    /// Empties the list of the states of interest.
    void reset() { states_.clear(); }

private:
    /// The list of the states of interest that should not be filtered out.
    std::vector<StateSharedPtr> states_;
};

/// Composite filter. Does not filter out only the
/// events not filtered out by any of the constituent filters.
/// \tparam Node The search node type.
template <class Node>
struct Filter: FilterBase<Node> {
    /// The type representing an event generated by the search algorithm.
    using Event = typename FilterBase<Node>::Event;

    /// Smart pointer to state type representing the domain.
    using StateSharedPtr = typename FilterBase<Node>::StateSharedPtr;

    /// Initializes the filter to only filter based on description strings of
    /// events.
    Filter(const std::vector<std::string> &strings)
        : filterEventStr_(strings),
          filters_({&filterEventStr_, &filterState_}) {}

    /// Returns the constituent filter that filters based on description strings
    /// of events.
    /// \return Reference to the constituent filter that filters based on
    /// description strings of events.
    FilterEventStr<Node> &filterEventStr() {
        return filterEventStr_;
    }

    /// Returns the constituent filter that filters based on the list of the
    /// states of interest.
    /// \return Reference to the constituent filter that filters based
    /// the list of the states of interest.
    FilterState<Node> &filterState() {
        return filterState_;
    }

    /// Determines whether the given event is not filtered out.
    /// \param e The event of interest.
    /// \return \c true if the given event is not filtered out.
    virtual bool in(const Event &e) const override {
        for (auto f : filters_)
            if (!f->in(e)) return false;
        return true;
    }
private:
    /// The constituent filter that filters based on description strings
    /// of events.
    FilterEventStr<Node> filterEventStr_;

    /// The constituent filter that filters based on the list of the
    /// states of interest.
    FilterState<Node> filterState_;

    /// The constituent filters.
    std::vector<FilterBase<Node> *> filters_;
};

} // namespace
} // namespace
} // namespace

#endif
