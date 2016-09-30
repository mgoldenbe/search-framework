/// \file
/// \brief The \ref SearchNode class.
/// \author Meir Goldenberg

#ifndef NODE_H
#define NODE_H

#include "managed_node.h"

/// The search node type. Inherits the data structure to make data (e.g. g- and
/// f-value) easy to access.
/// \tparam State_ The state type, represents the domain.
/// \tparam NodeData_ The structure storing the data kept by the node.
/// \tparam BucketPosition Type used for storing the node's position in the open
/// list (when applicable).
template <class State_ = SLB_STATE,
          class NodeData_ = SLB_NODE_DATA,
          typename BucketPosition = SLB_BUCKET_POSITION_TYPE>
struct SearchNode : public NodeData_ {
    /// The state type, represents the domain.
    using State = State_;

    /// The structure storing the data kept by the node.
    using NodeData = NodeData_;

    /// Type of the node.
    using MyType = SearchNode;

    /// Unique pointer to the node.
    using NodeUniquePtr = std::unique_ptr<MyType>;

    /// Initializes the node based on state.
    /// \param s The state.
    SearchNode(const State &s) : state_(s) {}

    /// Initializes the node based on state that is a right-value.
    /// \param s The state.
    SearchNode(State &&s) : state_(s) {}


    /// Returns reference to the state to which the node corresponds.
    /// \return Reference to the state to which the node corresponds.
    State &state() { return state_; }

    /// Returns const reference to the state to which the node corresponds.
    /// \return Const reference to the state to which the node corresponds.
    const State &state() const { return state_; }

    /// Assigns a different state to the node.
    /// \param s The state to assign.
    void setState(State &&s) { state_ = std::move(s); }

    /// Returns pointer to the parent node.
    /// \return Pointer to the parent node.
    MyType *parent() { return parent_; }

    /// Returns pointer to const parent node.
    /// \return Pointer to const parent node.
    const MyType *parent() const { return parent_; }

    /// Nodes comparison.
    /// \param rhs The node being compared to.
    /// \return \c true if the states corresponding to the nodes compare equal.
    bool operator==(const MyType &rhs) const {
        return state_ == rhs.state_;
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
        o << state_;
        if (parent) o << (parent->state);
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
    State state_; ///< The state corresponding to the node.
    MyType *parent_ = nullptr; ///< The parent node.

    /// The position of the node in the open list (when applicable).
    BucketPosition bucketPosition_;
};

#endif
