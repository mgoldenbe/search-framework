#ifndef ASTAR_EVENT
#define ASTAR_EVENT

#include <sstream>

template <class State_, class NodeData> struct AstarEvent {
    using State = State_;
    using StateSharedPtr = std::shared_ptr<const State>;
    using MyType = AstarEvent<State, NodeData>;

    enum class EventType {
        NOVAL,
        ROLE,
        BEGIN_GENERATE,
        END_GENERATE,
        SELECTED,
        SUSPENDED_EXPANSION,
        RESUMED_EXPANSION,
        DENIED_EXPANSION,
        CLOSED,
        CHANGED_PARENT
    };
    enum class StateRole { NOVAL, START, GOAL, DONE_GOAL };

    // ROLE event
    template <class Node>
    AstarEvent(const Node *n, const StateRole &role,
               const std::string &additional = "")
        : AstarEvent(n->shareState(), EventType::ROLE, role,
                     n->shareParentState(), additional) {}

    // NON-ROLE event
    template <class Node>
    AstarEvent(const Node *n, const EventType &type,
               const std::string &additional = "")
        : AstarEvent(n->shareState(), type, StateRole(),
                     n->shareParentState(), additional) {}

    const StateSharedPtr &state() const { return state_; }

    // -1 means no previous event
    void setLastEventStep(int step) { lastEventStep_ = step; }

    template <class AlgorithmLog>
    std::vector<StateSharedPtr> path(const StateSharedPtr &state,
                                     const AlgorithmLog &log) const {
        std::vector<StateSharedPtr> res;
        StateSharedPtr s = state;
        while (s) {
            res.push_back(s);
            s = log.getLastEvent(s).parent();
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    static std::ostream &dumpTitle(std::ostream &o) {
        o << std::setw(18) << "State" << std::setw(20) << "Event"
          << std::setw(12) << "Role" << std::setw(18) << "Parent"
          << std::setw(12) << "Note" << std::endl;
        return o;
    }

    std::ostream &dump(std::ostream &o) const {
        // There is an alternative here:
        // http://stackoverflow.com/a/23402871/2725810
        // Drawback: has to be declared outside of any class
        std::vector<std::string> eventTypeStr = {
            "NOVAL",             "ROLE",             "BEGIN_GENERATE",
            "END_GENERATE",      "SELECTED",         "SUSPENDED_EXPANSION",
            "RESUMED_EXPANSION", "DENIED_EXPANSION", "CLOSED",
            "CHANGED_PARENT"};
        std::vector<std::string> stateRoleStr = {"NOVAL", "START", "GOAL",
                                                 "DONE_GOAL"};
        std::ostringstream so;
        so << *state_;
        o  << std::setw(18) << so.str()
          << std::setw(20) << eventTypeStr[static_cast<int>(type_)];

        if (type_ == EventType::ROLE)
            o << std::setw(12) << stateRoleStr[static_cast<int>(role_)];
        else
            o << std::setw(12) << "n/a";

        if (parent_) {
            std::ostringstream so;
            so << *parent_;
            o << std::setw(18) << so.str();
        }
        else
            o << std::setw(18) << "n/a";
        o << std::setw(12) << additional_;
        return o;
    }

    void dump() const { dump(std::cerr); }
    const EventType &type() const {return type_;}
    const StateRole &role() const { return role_; }
    const StateSharedPtr &parent() const {return parent_;}

private:
    StateSharedPtr state_;
    const EventType type_;
    const StateRole role_;
    StateSharedPtr parent_;
    const std::string additional_;
    std::size_t lastEventStep_;

    AstarEvent(const StateSharedPtr &state, const EventType &type,
               const StateRole &role, const StateSharedPtr &parent,
               const std::string &additional)
        : state_(state), type_(type), role_(role), parent_(parent),
          additional_(additional) {}
};

template <class State, class NodeData>
std::ostream &operator<<(std::ostream &o,
                         const AstarEvent<State, NodeData> &e) {
    return e.dump(o);
}

#endif
