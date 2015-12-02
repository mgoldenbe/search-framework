#ifndef ASTAR_EVENT
#define ASTAR_EVENT

template <class State, class NodeData> struct AstarEvent {
    using StateSharedPtr = std::shared_ptr<const State>;
    using MyType = AstarEvent<State, NodeData>;

    enum class EventType { NOVAL, ROLE, SELECTED, GENERATED, CHANGED_PARENT };
    enum class StateRole { NOVAL, START, GOAL, DONE_GOAL };

    // ROLE event
    AstarEvent(const StateSharedPtr &state, const StateRole &role)
        : AstarEvent(state, EventType::ROLE, role, nullptr, NodeData()) {}

    // SELECTED event
    AstarEvent(const StateSharedPtr &state)
        : AstarEvent(state, EventType::SELECTED, StateRole(), nullptr,
                     NodeData()) {}

    // GENERATED event
    AstarEvent(const StateSharedPtr &state, const StateSharedPtr &parent,
               const NodeData &nodeData)
        : AstarEvent(state, EventType::GENERATED, StateRole(), parent,
                     nodeData) {}

    const StateSharedPtr &state() const { return state_; }

    // -1 means no previous event
    void setLastEventStep(int step) { lastEventStep_ = step; }

    std::ostream &dump(std::ostream &o) const {
        // There is an alternative here:
        // http://stackoverflow.com/a/23402871/2725810
        // Drawback: has to be declared outside of any class
        std::vector<std::string> eventTypeStr = {"NOVAL", "ROLE", "SELECTED",
                                                 "GENERATED", "CHANGED_PARENT"};
        std::vector<std::string> stateRoleStr = {"NOVAL", "START", "GOAL",
                                                 "DONE_GOAL"};
        o << *state_ << ": " << eventTypeStr[static_cast<int>(type_)] << ": ";
        switch (type_) {
        case EventType::ROLE:
            o << stateRoleStr[static_cast<int>(role_)];
            break;
        case EventType::SELECTED:
            break;
        case EventType::GENERATED:
            o << "Parent: " << *parent_ << "   Node: " << nodeData_;
            break;
        default:
            o << "Unhandled EventType" << std::endl;
            assert(0);
        }
        return o;
    }

    void dump() const { dump(std::cerr); }

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
