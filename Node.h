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

template <typename State, bool uniformFlag=true>
struct StateNeighbor {
    using StateUP = std::unique_ptr<State>;
    using CostType = typename State::CostType;
    StateNeighbor(State *s, CostType c) : scPair_(s, c) {}
    StateUP &state() {return scPair_.first;}
    CostType cost() const {return scPair_.second;}
private:
    std::pair<StateUP, CostType> scPair_;
};

template <typename State>
struct StateNeighbor<State, true> {
    using StateUP = std::unique_ptr<const State>;
    using CostType = typename State::CostType;
    StateNeighbor(State *s) : state_(s) {}
    StateUP &state() { return state_; }
    CostType cost() const {return 1;}
private:
    StateUP state_;
};


struct NoNodeData {};

template <typename State_, typename NodeData = NoNodeData,
          typename BucketPosition = int>
struct AStarNode : public NodeData {
    using State = State_;
    using StateUP = std::unique_ptr<const State>;
    using CostType = typename State::CostType;
    using MyType = AStarNode<State, NodeData>;
    using NodeUP = std::unique_ptr<MyType>;

    AStarNode(const State &s) : state_(new State(s)) {} // For testing only
    AStarNode(const State *s) : state_(s) {}            // For testing only
    AStarNode(StateUP &&s) : state_(s) {}

    template <typename State1, typename NodeData1>
    friend bool operator==(const AStarNode<State1, NodeData1> &n1,
                           const AStarNode<State1, NodeData1> &n2);

    /*
    std::vector<NodeUP> neighbors() const {
        auto stateNeighbors = StateUP.neighbors();
        std::vector<NodeUP> res;
        auto pos = -1;
        for (auto &s: stateNeighbors) {
            NodeUP cur(new MyType(std::move(s), g+1));
        }
        return res;
    }
    */
    const State &state() const { return *state_; }
    const BucketPosition &bucketPosition() const { return bucketPosition_; }
    void setBucketPosition(BucketPosition l) { bucketPosition_ = l; }
    CostType g = CostType(0), f = CostType(0); // These are public for ease of
                                               // access. I don't see any reason
                                               // for making them private and
                                               // having getters and setters.
private:
    StateUP state_;
    MyType *parent_ = nullptr;
    BucketPosition bucketPosition_;
};

template <typename State, typename NodeData>
std::ostream& operator<< (std::ostream& o, const AStarNode<State, NodeData> &n) {
    o << (n.state()) << " (" << "g: " << n.g << ", " << "f: " << n.f << ")";
    return o;
}

template <typename State, typename NodeData>
bool operator==(const AStarNode<State, NodeData> &n1,
                const AStarNode<State, NodeData> &n2) {
    return *(n1.state()) == *(n2.state());
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
