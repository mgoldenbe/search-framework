/// \file
/// \brief The \ref SearchNode class.
/// \author Meir Goldenberg

#ifndef NODE_H
#define NODE_H

#include "managed_node.h"

/// Alias for unique pointer to constant state.
/// \tparam State The state type, represents the domain.
template<typename State>
using StateUniquePtrT = std::unique_ptr<const State>;

/// Alias for shared pointer to constant state.
/// \tparam State The state type, represents the domain.
template<typename State>
using StateSharedPtrT = std::shared_ptr<const State>;

/// The search node type.
/// \tparam State_ The state type, represents the domain.
/// \tparam NodeData_ The structure storing the data kept by the node.
/// \tparam StateSmartPtrT The type for smart pointer to const state.
/// \tparam BucketPosition Type used for storing the node's position in the open
/// list (when applicable).
template <class State_ = Domains::SLB_STATE,
          class NodeData_ = Node::SLB_NODE_DATA,
          template <class> class StateSmartPtrT = SLB_STATE_SMART_PTR,
          typename BucketPosition = SLB_BUCKET_POSITION_TYPE>
struct SearchNode : public NodeData_ {
    /// The state type, represents the domain.
    using State = State_;

    /// The type for smart pointer to const state.
    using StateSmartPtr = StateSmartPtrT<State>;

    /// The structure storing the data kept by the node.
    using NodeData = NodeData_;

    /// Type of the node.
    using MyType = SearchNode<State, NodeData_, StateSmartPtrT, BucketPosition>;

    /// Unique pointer to the node.
    using NodeUniquePtr = std::unique_ptr<MyType>;

    /// Initializes the node based on state.
    /// \param s The state.
    SearchNode(const State &s) : state_(new State(s)) {}

    /// Initializes the node based on unique pointer to state.
    /// \param s Unique pointer to the state.
    SearchNode(StateUniquePtrT<State> &s) : state_(std::move(s)) {}

    /// Returns const reference to the state to which the node corresponds.
    /// \return Const reference to the state to which the node corresponds.
    const State &state() const { return *state_; }

    /// Returns pointer to the parent node (the latter is const).
    /// \return Pointer to the parent node (the latter is const).
    const MyType *parent() const { return parent_; }

    /// Nodes comparison.
    /// \param rhs The node being compared to.
    /// \return \c true if the states corresponding to the nodes compare equal.
    bool operator==(const MyType &rhs) const {
        return *(this->state()) == *(rhs.state());
    }

    /// Returns the state to which the node corresponds as a shared pointer.
    /// \return The state to which the node corresponds as a shared pointer.
    const StateSmartPtr &shareState() const { return state_; }

    /// Returns the state to which the parent node corresponds as a shared
    /// pointer.
    /// \return The state to which the parent node corresponds as a shared
    /// pointer. If there is no parent node, \c nullptr is returned.
    const StateSmartPtr &shareParentState() const {
        static StateSmartPtr nullResult = StateSmartPtr(nullptr);
        if (!parent_) return nullResult;
        return parent_->state_;
    }

    /// Computes the path by which the state to which the node corresponds was
    /// reached by the search.
    /// \return The path (as a vector of states) by which the state to which the
    /// node corresponds was reached by the search.
    const std::vector<State> path() const {
        std::vector<State> res;
        MyType *cur = this;
        while (cur) {
            res.push_back(cur->state());
            cur = cur->parent_;
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    /// Returns the position of the node in the open list (when applicable).
    /// \return The position of the node in the open list (when applicable).
    BucketPosition bucketPosition() const { return bucketPosition_; }

    /// Dumps the node (i.e. the state, the parent and the data) to the given
    /// stream.
    /// \tparam Stream The output stream type.
    /// \param o The output stream.
    template<typename Stream>
    Stream& dump(Stream& o) const {
        o << *state_;
        if (parent) o << *(parent->state);
        o << (NodeData)*this;
        return o;
    }

    /// Sets the parent node to the given node.
    /// \param parent The new parent node.
    void setParent(MyType *parent) { parent_ = parent; }

    /// Sets the position of the node in the open list (when applicable).
    /// \param l The new position of the node in the open list.
    void setBucketPosition(BucketPosition l) { bucketPosition_ = l; }

private:
    StateSmartPtr state_; ///< The state corresponding to the node.
    MyType *parent_ = nullptr; ///< The parent node.

    /// The position of the node in the open list (when applicable).
    BucketPosition bucketPosition_;
};

#endif
