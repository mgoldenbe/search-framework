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

template <typename CostType>
struct DefaultNodeData {
    CostType g, f;
    DefaultNodeData<CostType> *parent;
};

template <typename StateP, typename NodeData = DefaultNodeData>
struct AStarNode: public NodeData {
    using StateP = StateP;
    using NodeData = NodeData;
    StateP pstate;
};

template <typename AStarNodeP> struct AstarData {
    AStarNodeP getCurNode() const { return curNode; }
protected:
    AStarNodeP curNode;
};

template <typename StateP>
struct AStar {

}

main {
    if
        asd;
    asd;

}
