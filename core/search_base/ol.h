#ifndef HEAP_H
#define HEAP_H

/* TODO:
   -- faster update by using hints
*/

#include "node.h"

template <typename Key, typename T, class Compare = std::less<Key>>
using OLMap =
    std::map<Key, T, Compare, std::allocator<std::pair<const Key, T>>>;

template <class Node>
struct DefaultPriority {
    using CostType = typename Node::CostType;
    CostType g, f;
    DefaultPriority(const Node *n) : g(n->g), f(n->f) {}
};
template <class Node>
bool operator==(const DefaultPriority<Node> &p1,
                const DefaultPriority<Node> &p2) {
    return p1.f == p2.f && p1.g == p2.g;
}
template <class Node>
std::ostream& operator<< (std::ostream& o, const DefaultPriority<Node> &p) {
    o << " (" << "g: " << p.g << ", " << "f: " << p.f << ")";
    return o;
}

template <class Priority>
struct GreaterPriority_SmallF_LargeG {
    bool operator() (const Priority &p1, const Priority &p2) {
        if (p1.f < p2.f) return true;
        if (p1.f > p2.f) return false;
        if (p1.g > p2.g) return true;
        return false;
    }
};

template <class Priority>
struct GreaterPriority_SmallG {
    bool operator() (const Priority &p1, const Priority &p2) {
        if (p1.g < p2.g) return true;
        return false;
    }
};

template <class Node_ = SLB_NODE,
          template <class Node> class Priority_ = SLB_OL_PRIORITY_TYPE,
          template <class Priority> class GreaterPriority_ = SLB_OL_PRIORITY,
          template <typename, typename, typename> class Container =
              SLB_OL_CONTAINER>
struct OpenList {
    using BucketPosition = int; // must be defined by all SLB_OL variants
    using Node = Node_;
    using Priority = Priority_<Node>;
    using GreaterPriority = GreaterPriority_<Priority>;
    using CostType = typename Node::CostType;

    void add(Node *n) {
        auto &myBucket = buckets[Priority(n)];
        myBucket.push_back(n);
        n->setBucketPosition(myBucket.size() - 1);
        size_++;
    }

    std::size_t size() const { return size_; }

    bool empty() const {return size() == 0;}

    void update(Node *n, const Priority &oldPriority) {
        Priority newPriority(n);
        if (newPriority == oldPriority) {
            //std::cout << "Nothing needs to be done in update" << std::endl;
            return;
        }
        erase(oldPriority, n->bucketPosition());
        add(n);
    }

    Node *deleteMin() {
        return erase(buckets.begin()->first);
    }

    // the smallest possible f-value in the current bucket
    const Priority &curPriority() { return buckets.begin()->first; }

    void dump() const {
        for (auto b : buckets) {
            std::cout << b.first << ": ";
            for (auto n: b.second)
                std::cout << *n << "(bucketPos: " << n->bucketPosition() << ") ";
            std::cout << std::endl;
        }
    }

private:
    Container<Priority, std::vector<Node *>, GreaterPriority> buckets;
    int size_ = 0;
    Node *erase(const Priority &priority) {
        return erase(priority, buckets[priority].size()-1);
    }

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
