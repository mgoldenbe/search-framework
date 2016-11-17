#ifndef OPEN_LIST_H
#define OPEN_LIST_H

/// \file
/// \brief Open list implementations.
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace policy {

/// \namespace slb::ext::policy::openList
/// Open list implementations.
namespace openList {

using core::util::nodeStr;

/* TODO:
   -- faster update by using hints
*/

/// The underlying map type. Compared to std::map, this type fixes the allocator.
/// \tparam Key The key type.
/// \tparam T The value type.
/// \tparam Compare The functor type used to compare the keys.
template <typename Key, typename T, class Compare = std::less<Key>>
using OLMap =
    std::map<Key, T, Compare, std::allocator<std::pair<const Key, T>>>;

/// Nodes are ordered in the open list based on g and f. The particular ordering
/// rules are defined by the other policies.
/// \tparam Node The node type.
template <class Node>
struct DefaultOLKeyType {
    using CostType = typename Node::CostType; ///< Type for action cost.
    CostType g;                               ///< g-cost of the node.
    CostType f;                               ///< f-cost of the node.

    /// Initializes the key based on the node.
    /// \param n The node.
    DefaultOLKeyType(const Node *n) : g(n->g), f(n->f) {}
};

/// Comparison of two keys.
/// \tparam The node type.
/// \param lhs The left-hand side key in the comparison.
/// \param rhs The right-hand side key in the comparison.
/// \return \c true if the keys compare equal and \c false otherwise.
template <class Node>
bool operator==(const DefaultOLKeyType<Node> &lhs,
                const DefaultOLKeyType<Node> &rhs) {
    return lhs.f == rhs.f && lhs.g == rhs.g;
}

/// Output the given key to a given stream.
/// \tparam Stream The stream type.
/// \tparam Node The node type.
/// \param o The stream.
/// \param key The key.
/// \return The modified stream.
template <class Stream, class Node>
Stream& operator<< (Stream& o, const DefaultOLKeyType<Node> &key) {
    o << " (" << "g: " << key.g << ", " << "f: " << key.f << ")";
    return o;
}

/// Functor that compares keys based on f-value and breaks ties in favor of
/// larger g-value.
/// \tparam KeyType The key type.
/// \param lhs The left-hand side key in the comparison.
/// \param rhs The right-hand side key in the comparison.
/// \return \c true if \c lhs is higher priority than \c rhs and \c false
/// otherwise.
template <class KeyType>
struct GreaterPriority_SmallF_LargeG {
    /// Compares two keys.
    /// \param lhs First key.
    /// \param rhs Second key.
    /// \return \c true if \c lhs precedes \c rhs and \c false otherwise.
    bool operator() (const KeyType &lhs, const KeyType &rhs) {
        if (lhs.f < rhs.f) return true;
        if (lhs.f > rhs.f) return false;
        if (lhs.g > rhs.g) return true;
        return false;
    }
};

/// Functor that compares keys based on g-value and prioritizes in favor of a
/// smaller g-value.
/// \tparam KeyType The key type.
/// \param lhs The left-hand side key in the comparison.
/// \param rhs The right-hand side key in the comparison.
/// \return \c true if \c lhs is higher priority than \c rhs and \c false
/// otherwise.
template <class KeyType>
struct GreaterPriority_SmallG {
    /// Compares two keys.
    /// \param lhs First key.
    /// \param rhs Second key.
    /// \return \c true if \c lhs precedes \c rhs and \c false otherwise.
    bool operator() (const KeyType &lhs, const KeyType &rhs) {
        if (lhs.g < rhs.g) return true;
        return false;
    }
};

/// A flexible open list base on \c std::map whose values are buckets of nodes
/// with same priority.
/// \tparam MyAlgorithm_ The search algorithm type.
/// \tparam Node_ The node type.
/// \tparam KeyType_ The key type.
/// \tparam GreaterPriority_ The functor type used to compare the keys.
/// \tparam The underlying container.
template <class MyAlgorithm_, class Node_ = SLB_NODE,
          template <class Node> class KeyType_ = SLB_OL_KEY_TYPE,
          template <class KeyType> class GreaterPriority_ = SLB_OL_PRIORITY,
          template <typename, typename, typename> class Container =
              SLB_OL_CONTAINER>
struct BucketedStdMap_T {
    /// The search algorithm type.
    using MyAlgorithm = MyAlgorithm_;

    /// Type for storing node position in the open list.
    using BucketPosition = int;

    /// The node type.
    using Node = Node_;

    /// The key type.
    using KeyType = KeyType_<Node>;

    /// The functor type used to compare the keys.
    using GreaterPriority = GreaterPriority_<KeyType>;

    /// Type for action cost in the search domain.
    using CostType = typename Node::CostType;

    /// Type of the container for storing buckets of nodes with same priority.
    using BucketsContainer =
        Container<KeyType, std::vector<Node *>, GreaterPriority>;

    /// The constructor.
    /// \param alg Reference to the search algorithm.
    BucketedStdMap_T(MyAlgorithm &alg): alg_(alg) {
#ifndef NDEBUG
        // just to prevent the compiler from throwing it away,
        // so it should be available for use in gdb.
        dump();
#endif
    }

    /// Adds the given node to the list.
    /// \param n Pointer to the node to be added.
    void add(Node *n) {
        auto &myBucket = buckets_[KeyType(n)];
        myBucket.push_back(n);
        n->setBucketPosition(myBucket.size() - 1);
        size_++;
    }

    /// Returns the number of nodes in the list.
    /// \return The number of nodes in the list.
    std::size_t size() const { return size_; }

    /// Returns \c true if the list is empty and \c false otherwise.
    /// \return \c true if the list is empty and \c false otherwise.
    bool empty() const {return size() == 0;}

    /// Updates the priority of the given node.
    /// If the node is not in the open list, it is added whether or not it's key
    /// has changed.
    /// \param n Pointer to the node whose priority has changed.
    /// \param oldPriority The priority that \c n used to have.
    void update(Node *n, const KeyType &oldPriority) {
        KeyType newPriority(n);
        auto b = n->bucketPosition();
        if (newPriority == oldPriority && b != -1) {
            //std::cout << "Nothing needs to be done in update" << std::endl;
            return;
        }
        if (b != -1) erase(oldPriority, n->bucketPosition());
        add(n);
    }

    /// Removes the highest priority node from the list and returns the former.
    /// \return Pointer to the highest priority node.
    Node *deleteMin() {
        return erase(buckets_.begin()->first);
    }

    /// Returns the highest priority without removing the corresponding node.
    /// \return Const reference to the highest priority in the list.
    const KeyType &curPriority() { return buckets_.begin()->first; }

    /// Dumps the list to \c stderr for debugging.
    void dump() const {
        for (auto b : buckets_) {
            std::cerr << b.first << ":\n";
            for (auto n: b.second)
                std::cerr << "    " << n->state() << " " << nodeStr(*n, 0)
                          << "(bucketPos: " << n->bucketPosition() << ") "
                          << std::endl;
            std::cerr << std::endl;
        }
    }

    /// Re-compute the whole open list
    /// \note It is faster to put all map elements into a vector and sort them
    /// there first, but it is not clear how to map keys to buckets_.size()
    /// vector indices. We could insert into un-ordered map first, but then we
    /// lose the performance benefits (checked with a simple prototype).
    void recompute() {
        BucketedStdMap_T newOL(alg_);
        for (const auto &b: buckets_) {
            for (auto n: b.second) {
                n->updateH(alg_.heuristic()(n));
                newOL.add(n);
            }
        }
        std::swap(newOL.buckets_, buckets_);
        assert(newOL.size() == size_);
    }
private:
    /// Reference to the search algorithm.
    MyAlgorithm &alg_;

    /// The underlying map. Nodes with same priority are kept in buckets. These
    /// buckets are the values in the map.
    BucketsContainer buckets_;

    /// Number of nodes in the list.
    int size_ = 0;

    /// Removes from the list the last node in the bucket corresponding to the
    /// given priority and returns the former.
    /// \param priority The bucket from which the last node needs to be removed.
    /// \return Pointer to the node being removed.
    Node *erase(const KeyType &priority) {
        return erase(priority, buckets_[priority].size()-1);
    }

    /// Removes from the list the node with the given position in the bucket
    /// corresponding to the given priority and returns the former.
    /// \param priority The bucket from which the last node that needs to be
    /// removed.
    /// \param pos The position in the bucket of the node that needs to be
    /// removed.
    /// \return Pointer to the node being removed.
    Node *erase(const KeyType &priority, const BucketPosition &pos) {
        auto &bucket = buckets_[priority];
        assert(bucket.size());

        auto res = bucket[pos];

        // copy the last node of the bucket here
        bucket[pos] = bucket.back();
        bucket[pos]->setBucketPosition(pos);

        // remove the last node of the bucket
        bucket.pop_back();
        if (bucket.empty()) buckets_.erase(priority);

        size_--;
        res->setBucketPosition(-1);
        return res;
    }
};

/// A flexible open list base on \c std::map whose values are buckets of nodes
/// with same priority.
/// \tparam MyAlgorithm The search algorithm type.
template <class MyAlgorithm>
using BucketedStdMap = BucketedStdMap_T<MyAlgorithm>;

} // namespace
} // namespace
} // namespace
} // namespace

#endif
