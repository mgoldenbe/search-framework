#ifndef NODE
#define NODE

/* Design issues:
   1. On the one hand, generation of neighbors is logically the operation of State. On the other hand, it produces costs that need to be taken into accout when forming nodes. In addition, forming a vector of nodes from a vector of states has an overhead.
   2. State logically pertains to the domain, while g pertains to search, so it does not make logical sense to put g together with State.
   3. We need to enable different kinds of node information, e.g. nodes that maintain both static and stored f-values.
   4. Storing a pointer to State with Node adds a level of indirection.

   Solution 1:
   1. Note that, once the node is generated, we access State only to generate it's neighbors. The cost of indirection for this access is very small compared to the actual generation of the neighbots. Hence, we can afford storing the pointer to State with the node.
   2. Assigning a state pointer is not a large overhead, so we'll put up with it for now.
   3. We'll define a type for storing both a state and a cost of the transfer to this state from the parent.
*/

template <typename State_, bool uniformFlag=true>
struct StateNeighbor {
    using State = State_;
    using StateUP = std::unique_ptr<State>;
    using CostType = typename State::CostType;
    StateNeighbor(State *s, CostType c) : scPair_(s, c) {}
    StateUP &state() {return scPair_.first;}
    State stateCopy() const { return *(scPair_.first); }
    CostType cost() const {return scPair_.second;}
private:
    std::pair<StateUP, CostType> scPair_;
};

template <typename State_>
struct StateNeighbor<State_, true> {
    using State = State_;
    using StateUP = std::unique_ptr<const State>;
    using CostType = typename State::CostType;
    StateNeighbor(State *s) : state_(s) {}
    StateUP &state() { return state_; }
    State stateCopy() const { return *state_; }
    CostType cost() const {return 1;}
private:
    StateUP state_;
};

template <typename State_>
struct NodeBase {
    using State = State_;
    using CostType = typename State::CostType;
    CostType g = CostType(0), f = CostType(0); // These are public for ease of
                                               // access. I don't see any reason
                                               // for making them private and
                                               // having getters and setters.
    std::ostream &dump(std::ostream &o) const {
        o << " (" << "g: " << g << ", " << "f: " << f << ")";
        return o;
    }
};
template <typename State>
std::ostream &operator<< (std::ostream &o, const NodeBase<State> &n) {
    return n.dump(o);
}

template <typename State_>
struct NoNodeData: public NodeBase<State_> {};

template<typename State>
using UniqueStatePtr = std::unique_ptr<const State>;
template<typename State>
using SharedStatePtr = std::shared_ptr<const State>;
template<typename State>
using DefaultStatePtr = UniqueStatePtr<const State>;

template <typename State_, template <class> class NodeData_ = NoNodeData,
          template <class> class SmartStatePtr = DefaultStatePtr,
          typename BucketPosition = int>
struct AStarNode : public NodeData_<State_> {
    using State = State_;
    using StateSP = SmartStatePtr<const State>;
    using NodeData = NodeData_<State>;
    using MyType = AStarNode<State, NodeData_, SmartStatePtr, BucketPosition>;
    using NodeUP = std::unique_ptr<MyType>;

    AStarNode(const State &s) : state_(new State(s)) {} // For testing only
    AStarNode(const State *s) : state_(s) {}            // For testing only
    AStarNode(UniqueStatePtr<State> &s) : state_(std::move(s)) {}

    bool operator==(const MyType &rhs) {
        return *(this->state()) == *(rhs.state());
    }

    const State &state() const { return *state_; }
    const BucketPosition &bucketPosition() const { return bucketPosition_; }
    void setBucketPosition(BucketPosition l) { bucketPosition_ = l; }

    std::ostream& dump(std::ostream& o) const {
        o << *state_ << (NodeData)*this;
        return o;
    }

    void dump() const {
        dump(std::cerr);
    }
private:
    StateSP state_;
    MyType *parent_ = nullptr;
    BucketPosition bucketPosition_;
};

template <typename State, template <class> class NodeData = NoNodeData,
          template <class> class SmartStatePtr = DefaultStatePtr,
          typename BucketPosition = int>
std::ostream &operator<<(
    std::ostream &o,
    const AStarNode<State, NodeData, SmartStatePtr, BucketPosition> &n) {
    return n.dump(o);
}

/*
namespace std {
    template<typename State, typename NodeData>
    struct hash<AStarNode<State, NodeData>> {
        std::size_t operator ()(AStarNode<State, NodeData> const& key) const {
            return key.state()->hash();
        }
    };
}
*/

#endif
