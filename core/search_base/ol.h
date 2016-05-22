#ifndef HEAP_H
#define HEAP_H

/// \file
/// \brief A flexible open list implementation.
/// \author Meir Goldenberg

/* TODO:
   -- faster update by using hints
*/

#include "node.h"

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
/// \tparam Priority The key type.
/// \param lhs The left-hand side key in the comparison.
/// \param rhs The right-hand side key in the comparison.
/// \return \c true if \c lhs is higher priority than \c rhs and \c false
/// otherwise.
template <class Priority>
struct GreaterPriority_SmallF_LargeG {
    bool operator() (const Priority &lhs, const Priority &rhs) {
        if (lhs.f < rhs.f) return true;
        if (lhs.f > rhs.f) return false;
        if (lhs.g > rhs.g) return true;
        return false;
    }
};

/// Functor that compares keys based on g-value and prioritizes in favor of a
/// smaller g-value.
/// \tparam Priority The key type.
/// \param lhs The left-hand side key in the comparison.
/// \param rhs The right-hand side key in the comparison.
/// \return \c true if \c lhs is higher priority than \c rhs and \c false
/// otherwise.
template <class Priority>
struct GreaterPriority_SmallG {
    bool operator() (const Priority &lhs, const Priority &rhs) {
        if (lhs.g < rhs.g) return true;
        return false;
    }
};

/// A flexible open list base on \c std::map whose values are buckets of nodes
/// with same priority.
/// \tparam Node_ The node type.
/// \tparam Priority_ The key type.
/// \tparam GreaterPriority_ The functor type used to compare the keys.
/// \tparam The underlying container.
template <class Node_ = SLB_NODE,
          template <class Node> class Priority_ = SLB_OL_KEY_TYPE,
          template <class Priority> class GreaterPriority_ = SLB_OL_PRIORITY,
          template <typename, typename, typename> class Container =
              SLB_OL_CONTAINER>
struct OpenList {
    /// Type for storing node position in the open list.
    using BucketPosition = int;

    /// The node type.
    using Node = Node_;

    /// The key type.
    using Priority = Priority_<Node>;

    /// The functor type used to compare the keys.
    using GreaterPriority = GreaterPriority_<Priority>;

    /// Type for action cost.
    using CostType = typename Node::CostType;

    /// Adds the given node to the list.
    /// \param n Pointer to the node to be added.
    void add(Node *n) {
        auto &myBucket = buckets[Priority(n)];
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
    /// \param n Pointer to the node whose priority has changed.
    /// \param oldPriority The priority that \c n used to have.
    void update(Node *n, const Priority &oldPriority) {
        Priority newPriority(n);
        if (newPriority == oldPriority) {
            //std::cout << "Nothing needs to be done in update" << std::endl;
            return;
        }
        erase(oldPriority, n->bucketPosition());
        add(n);
    }

    /// Removes the highest priority node from the list and returns the former.
    /// \return Pointer to the highest priority node.
    Node *deleteMin() {
        return erase(buckets.begin()->first);
    }

    /// Returns the highest priority without removing the corresponding node.
    /// \return Const reference to the highest priority in the list.
    const Priority &curPriority() { return buckets.begin()->first; }

    /// Dumps the list to \c stderr for debugging.
    void dump() const {
        for (auto b : buckets) {
            std::cerr << b.first << ": ";
            for (auto n: b.second)
                std::cerr << *n << "(bucketPos: " << n->bucketPosition() << ") ";
            std::cerr << std::endl;
        }
    }

private:
    /// The underlying map. Nodes with same priority are kept in buckets. These
    /// buckets are the values in the map.
    Container<Priority, std::vector<Node *>, GreaterPriority> buckets;

    /// Number of nodes in the list.
    int size_ = 0;

    /// Removes from the list the last node in the bucket corresponding to the
    /// given priority and returns the former.
    /// \param priority The bucket from which the last node needs to be removed.
    /// \return Pointer to the node being removed.
    Node *erase(const Priority &priority) {
        return erase(priority, buckets[priority].size()-1);
    }

    /// Removes from the list the node with the given position in the bucket
    /// corresponding to the given priority and returns the former.
    /// \param priority The bucket from which the last node that needs to be
    /// removed.
    /// \param pos The position in the bucket of the node that needs to be
    /// removed.
    /// \return Pointer to the node being removed.
    Node *erase(const Priority &priority, const BucketPosition &pos) {
        auto &bucket = buckets[priority];
        auto res = bucket[pos];

        // copy the last node of the bucket here
        bucket[pos] = bucket.back();
        bucket[pos]->setBucketPosition(pos);

        // remove the last node of the bucket
        bucket.pop_back();
        if (bucket.empty()) buckets.erase(priority);

        // the client code wants to take care of it
        // remember to re-institute this line in the generic library B"H
        // res->heapIndex = -1;
        size_--;
        return res;
    }
};

#endif
