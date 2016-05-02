#ifndef EVENT_BASE_H
#define EVENT_BASE_H

#include "algorithm_log.h"
#include "current_styles.h"

namespace Events {

template <class Node = SLB_NODE> struct Base {
    using State = typename Node::State;
    using StateSharedPtr = std::shared_ptr<const State>;
    using NodeData = typename Node::NodeData;
    using Event = std::shared_ptr<Events::Base<Node>>;

    struct VertexChange {
        using StateSharedPtr = std::shared_ptr<const State>;
        StateSharedPtr s;
        VertexStyle now;
    };

    struct ArcChange {
        using StateSharedPtr = std::shared_ptr<const State>;
        StateSharedPtr from, to;
        EdgeStyle now;
    };

    struct VisualChanges {
        std::vector<VertexChange> vChanges;
        std::vector<ArcChange> aChanges;
    };

    ///@name Construction and Assignment
    //@{
    Base(const AlgorithmLog<Node> &logger, const Node *n)
        : logger_(logger), state_(n->shareState()),
          parent_(n->shareParentState()), nodeData_(*n), step_(logger.size()),
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
    NodeData nodeData_;
    int step_;
    Event previousEvent_; // last event with the same state
};

}
#endif
