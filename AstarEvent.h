///@file
///@brief INTERFACES CHECKED.

#ifndef ASTAR_EVENT
#define ASTAR_EVENT

#include "AlgorithmLogger.h"

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
    static std::vector<std::string> eventTypeStr;

    enum class StateRole { NOVAL, START, GOAL, BEGIN_DONE_GOAL, END_DONE_GOAL };
    // There is an alternative here:
    // http://stackoverflow.com/a/23402871/2725810
    // Drawback: has to be declared outside of any class
    static std::vector<std::string> stateRoleStr;

    ///@name Construction and Assignment
    //@{
    template <class Node>
    AstarEvent(const AlgorithmLogger<AstarEvent> &logger, const Node *n,
               EventType type, StateRole role = StateRole::NOVAL)
        : logger_(logger), state_(n->shareState()), type_(type), role_(role),
          parent_(n->shareParentState()), nodeData_(*n) {}
    //@}

    ///@name Read-Only Services
    //@{
    int step() const { return step_; }
    const AlgorithmLogger<AstarEvent> &logger() const { return logger_; }
    const StateSharedPtr &state() const { return state_; }
    const MyType &lastEvent() const { // last event for the same state
        return logger_.event(lastEventStep_);
    }
    const EventType &type() const { return type_; }
    const StateRole &role() const { return role_; }
    const StateSharedPtr &parent() const { return parent_; }
    const NodeData &nodeData() const { return nodeData_; }

    std::vector<StateSharedPtr> path(const StateSharedPtr &state) const {
        std::vector<StateSharedPtr> res;
        StateSharedPtr s = state;
        while (s) {
            res.push_back(s);
            s = logger_.getLastEvent(s).parent();
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    template <class Stream> // can be Table
    Stream &dump(Stream &o) const {
        o << std::setw(6) << step_;
        o << std::setw(18) << str(*state_) << std::setw(20)
          << eventTypeStr[static_cast<int>(type_)];

        if (type_ == EventType::ROLE)
            o << std::setw(12) << stateRoleStr[static_cast<int>(role_)];
        else
            o << std::setw(12) << "n/a";

        if (parent_)
            o << std::setw(18) << str(*parent_);
        else
            o << std::setw(18) << "n/a";
        o << std::setw(12) << nodeData_;
        return o;
    }
    //@}

    //template <typename CharT>
    //std::basic_ostream<CharT> &dump(std::basic_ostream<CharT> &o) const {

    ///@name Modification
    //@{
    // lastEventStep=-1 means no previous event
    void setStep(int step, int lastEventStep) {
        step_ = step;
        lastEventStep_ = lastEventStep;
    }
    //@}

    template <class Stream> // can be Table
    static Stream &dumpTitle(Stream &o) {
        o << std::setw(6) << "Step" << std::setw(18) << "State" << std::setw(20)
          << "Event" << std::setw(12) << "Role" << std::setw(18) << "Parent"
          << std::setw(12) << "Note" << std::endl;
        return o;
    }
private:
    const AlgorithmLogger<AstarEvent> &logger_;
    StateSharedPtr state_;
    const EventType type_;
    const StateRole role_;
    StateSharedPtr parent_;
    NodeData nodeData_;
    int step_;
    int lastEventStep_;
};

template <class State, class NodeData>
std::ostream &operator<<(std::ostream &o,
                         const AstarEvent<State, NodeData> &e) {
    return e.dump(o);
}

template <class State, class NodeData>
std::vector<std::string> AstarEvent<State, NodeData>::eventTypeStr = {
        "NOVAL",             "ROLE",             "BEGIN_GENERATE",
        "END_GENERATE",      "SELECTED",         "SUSPENDED_EXPANSION",
        "RESUMED_EXPANSION", "DENIED_EXPANSION", "CLOSED",
        "CHANGED_PARENT"};

template <class State, class NodeData>
std::vector<std::string> AstarEvent<State, NodeData>::stateRoleStr = {
    "NOVAL", "START", "GOAL", "BEGIN_DONE_GOAL", "END_DONE_GOAL"};
#endif
