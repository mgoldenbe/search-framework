#ifndef SLB_CORE_SEARCH_BASE_OPEN_CLOSED_LIST_H
#define SLB_CORE_SEARCH_BASE_OPEN_CLOSED_LIST_H

/// \file
/// \brief The \ref slb::core::sb::OpenClosedList class.
/// \author Meir Goldenberg

namespace slb {
namespace core {
namespace sb {

// See here the discussion of whether nodes should be passed by pointers or by
// references: http://stackoverflow.com/q/33616355/2725810
//
// A quote from the comment by the author of the accepted answer:
// "Implementation detail is what you call an implementation detail. If your
// container goal is to store pointers (as method add assumes), storing pointers
// is hardly irrelevant for outside world."
//
// A useful quote for another answer: "references are better used as transparent
// aliases that don't have to live for very long."
/// The type for storing open and closed lists of \c A* family of algorithms.
/// \tparam OpenList The underlying open list's type.
/// \note The state type needs to implement moving copy constructor for
/// insertions into the list to be efficient.
template <class OpenList> struct OpenClosedList {
    /// The search algorithm.
    using MyAlgorithm = typename OpenList::MyAlgorithm;

    /// The node type.
    using Node = typename OpenList::Node;

    /// The key type of the open list.
    using KeyType = typename OpenList::KeyType;

    /// Type for action cost in the search domain.
    using CostType = typename Node::CostType;

    /// The type for unique pointer to search node.
    using NodeUniquePtr = typename Node::NodeUniquePtr;

    /// The state type, represents the domain.
    using State = typename Node::State;

    /// The type of the underlying hash table for the closed list.
    using HashType =
        std::unordered_map<State, NodeUniquePtr, util::StateHash<State>>;

    /// The constructor.
    /// \param alg Reference to the search algorithm.
    OpenClosedList(MyAlgorithm &alg) : ol_(alg) {}

    /// Dumps the list to \c stderr for debugging.
    void dump() const {
        std::cerr << "-----------------" << std::endl;
        std::cerr << "Dumping OpenClosedList:" << std::endl;
        std::cerr << "-----------------" << std::endl;
        std::cerr << "OL: " << ol_.size() << " elements" << std::endl;
        ol_.dump();
        std::cerr << "-----------------" << std::endl;
        std::cerr << "CL: " << hash_.size() << " elements" << std::endl;
        for (auto it = hash_.begin(); it != hash_.end(); ++it)
            std::cerr << *(it->second) << std::endl;
        std::cerr << "-----------------" << std::endl << std::endl;
    }

    /// Returns \c true if the list is empty and \c false otherwise.
    /// \return \c true if the list is empty and \c false otherwise.
    bool empty() const { return ol_.empty(); }

    /// Finds a node corresponding to the given state in the list.
    /// \param s The state of interest.
    /// \return Non-const pointer to the node corresponding to \c s.
    /// \note Returning a non-const pointer is unsafe, but I do not have a
    /// better solution for now. The user needs to be able to update g and f, to
    /// call update() and to call close(). The danger is that he can forget to
    /// call update().
    Node *getNode(const State &s) {
        auto it = hash_.find(s);
        if (it == hash_.end()) return nullptr;
        return (it->second).get();
    }

    /// Updates the priority of the given node.
    /// \param n Pointer to the node whose priority has changed.
    /// \param oldPriority The priority that \c n used to have.
    void update(Node *n, const KeyType &oldPriority) {
        ol_.update(n, oldPriority);
    }

    /// Adds the given node to the list.
    /// \param n Unique pointer to the node to be added.
    void add(NodeUniquePtr &n) {
        ol_.add(n.get());
        auto key = n->state();
        hash_[key] = std::move(n);
    }

    /// Re-inserts the given node to the list. Since the node is already owned
    /// by the list, the algorithm passes the pointer in this case, unlike for
    /// \ref add.
    /// \param n Pointer to the node to be re-inserted.
    void reInsert(Node *n) {
        ol_.add(n);
    }

    /// Removes the highest priority node from the list and returns the former.
    /// \return Pointer to the highest priority node.
    Node *minNode() { return ol_.deleteMin(); }

    /// Closes the given node.
    /// \param n Pointer to the node to be closed.
    void close(Node *n) { n->setBucketPosition(-1); }

    /// Returns the underlying hash table of the closed list.
    /// \return Const reference to the underlying hash table of the closed list.
    const HashType &hash() const { return hash_; }

    template <typename P>
    /// Update the nodes of the open list for which the predicate holds.
    void partialRecomputeOpen(const P &p) {
        ol_.partialRecompute(p);
    }

    /// Re-compute the whole open list
    void recomputeOpen() {
        ol_.recompute();
    }

    /// Returns the priority in the open list of the given node.
    /// \return The priority in the open list of the given node.
    KeyType priority(Node *n) {
        return KeyType(n);
    }

    /// Return the reference to the open list.
    /// \return The reference to the open list.
    OpenList &ol() { return ol_; }
private:
    OpenList ol_; ///< The open list.
    HashType hash_; ///< The closed list.
};

} // namespace
} // namespace
} // namespace

#endif
