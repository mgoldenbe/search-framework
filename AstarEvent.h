#ifndef ASTAR_EVENT
#define ASTAR_EVENT

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
        CLOSED,
        CHANGED_PARENT
    };
    enum class StateRole { NOVAL, START, GOAL, DONE_GOAL };

    // ROLE event
    AstarEvent(const StateSharedPtr &state, const StateRole &role,
               const StateSharedPtr &parent)
        : AstarEvent(state, EventType::ROLE, role, parent, NodeData()) {}

    // SELECTED or CLOSED
    AstarEvent(const StateSharedPtr &state, EventType type,
               const StateSharedPtr &parent)
        : AstarEvent(state, type, StateRole(), parent, NodeData()) {}

    // BEGIN_GENERATE or END_GENERATE
    AstarEvent(const StateSharedPtr &state, EventType type,
               const StateSharedPtr &parent, const NodeData &nodeData)
        : AstarEvent(state, type, StateRole(), parent,
                     nodeData) {}

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

    std::ostream &dump(std::ostream &o) const {
        // There is an alternative here:
        // http://stackoverflow.com/a/23402871/2725810
        // Drawback: has to be declared outside of any class
        std::vector<std::string> eventTypeStr = {
            "NOVAL",    "ROLE",   "BEGIN_GENERATE", "END_GENERATE",
            "SELECTED", "CLOSED", "CHANGED_PARENT"};
        std::vector<std::string> stateRoleStr = {"NOVAL", "START", "GOAL",
                                                 "DONE_GOAL"};
        o << *state_ << ": " << eventTypeStr[static_cast<int>(type_)] << ": ";
        switch (type_) {
        case EventType::ROLE:
            o << stateRoleStr[static_cast<int>(role_)];
            break;
        case EventType::SELECTED:
            break;
        case EventType::BEGIN_GENERATE: case EventType::END_GENERATE:
            o << "Parent: " << *parent_ << "   Node: " << nodeData_;
            break;
        case EventType::CLOSED:
            break;
        default:
            o << "Unhandled EventType";
            assert(0);
        }
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
    const NodeData nodeData_;
    std::size_t lastEventStep_;

    AstarEvent(const StateSharedPtr &state, const EventType &type,
               const StateRole &role, const StateSharedPtr &parent,
               const NodeData &nodeData)
        : state_(state), type_(type), role_(role), parent_(parent),
          nodeData_(nodeData) {}
};

template <class State, class NodeData>
std::ostream &operator<<(std::ostream &o,
                         const AstarEvent<State, NodeData> &e) {
    return e.dump(o);
}

#endif
