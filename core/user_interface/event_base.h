#ifndef EVENT_BASE_H
#define EVENT_BASE_H

/// \file
/// \brief The base class for the events generated by the search algorithm.
/// \author Meir Goldenberg

#include "algorithm_log.h"
#include "current_styles.h"

/// All the names related to the events generated by the search algorithm.
namespace Events {

enum class EventType {
    NORMAL,
    HIDE_LAST_EVENT
};

/// The base class for the events generated by the search algorithm.
template <class Node = SLB_NODE> struct Base {
    using State = typename Node::State;
    using StateSharedPtr = std::shared_ptr<const State>;
    using NodeData = typename Node::NodeData;

    /// Smart pointer to an event generated by the search algorithm.
    using Event = std::shared_ptr<Events::Base<Node>>;

    struct VertexChange {
        using StateSharedPtr = std::shared_ptr<const State>;
        StateSharedPtr s;
        VertexStyle now;
        VertexStyle before;
    };

    struct ArcChange {
        using StateSharedPtr = std::shared_ptr<const State>;
        StateSharedPtr from, to;
        EdgeStyle now;
        EdgeStyle before;
    };

    struct VisualChanges {
        std::vector<VertexChange> vChanges;
        std::vector<ArcChange> aChanges;
    };

    ///@name Construction and Assignment
    //@{
    Base(const AlgorithmLog<Node> &logger, const Node *n,
         const Node *parentSubstitution = nullptr)
        : logger_(logger), state_(n->shareState()),
          parent_(n->shareParentState()),
          parentSubstitution_(
              parentSubstitution ? parentSubstitution->shareState() : nullptr),
          nodeData_(*n), step_(logger.size()),
          previousEvent_(logger.getLastEvent(state_, false)) {}
    //@}

    ///@name Read-Only Services
    //@{
    int step() const { return step_; }
    const AlgorithmLog<Node> &logger() const { return logger_; }
    const StateSharedPtr &state() const { return state_; }
    const Event previousEvent() const { // last event for the same state
        return previousEvent_;
    }
    const StateSharedPtr &parent() const { return parent_; }
    const NodeData &nodeData() const { return nodeData_; }
    virtual std::string eventStr() const = 0;
    virtual EventType eventType() const { return EventType::NORMAL; }

    std::vector<StateSharedPtr> path(const StateSharedPtr &state) const {
        std::vector<StateSharedPtr> res;
        StateSharedPtr s = state;
        while (s) {
            res.push_back(s);
            s = logger_.getLastEvent(s)->parent();
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    virtual VisualChanges visualChanges(const CurrentStyles<State> &) const {
        return VisualChanges();
    }

    template <class Stream> // should be Table
    Stream &dump(Stream &o) const {
        o << std::setw(6) << step_;
        o << std::setw(18) << str(*state_) << std::setw(32) << eventStr();

        if (parent_)
            o << std::setw(18) << str(*parent_);
        else
            o << std::setw(18) << "n/a";
        o << std::setw(12) << nodeData_;
        return o;
    }

    /// Dumps to stderr
    void dump() const {
        std::cerr << step_ << " " << eventStr() << "    State: " << str(*state_)
                  << "   Parent: " << (parent_ ? str(*parent_) : "none")
                  << "   Subst.: "
                  << (parentSubstitution_ ? str(*parentSubstitution_) : "none")
                  << std::endl;
    }
    //@}

    template <class Stream> // can be Table
    static Stream &dumpTitle(Stream &o) {
        o << std::setw(6) << "Step" << std::setw(18) << "State" << std::setw(32)
          << "EventStr" << std::setw(18) << "Parent" << std::setw(12) << "Note"
          << std::endl;
        return o;
    }
protected:
    const AlgorithmLog<Node> &logger_;
    StateSharedPtr state_;
    StateSharedPtr parent_;
    StateSharedPtr parentSubstitution_; ///> Sometimes not the real parent, but
                                        ///another state needs to be used in
                                        ///visualization, e.g. when a duplicate
                                        ///path is detected.
    NodeData nodeData_;
    int step_;
    Event previousEvent_; // last event with the same state
};

}
#endif
