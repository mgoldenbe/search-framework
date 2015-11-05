#ifndef HEAP
#define HEAP

#include <iostream>
#include <vector>
#include <map>
#include "utilities.h"

//#include "prettyprint.h"

/* TODO:
   -- ability to break ties based on g-value
   -- faster update by using hints
*/

template <typename Node_, typename FType // Possible to achieve bucketing by
                                         // specifying Bucketed
                                         // By default: CostType of the node
                          = typename Node_::CostType>
struct OL {
    using OLLocation = int; // must be defined by all OL variants
    using Node = Node_;
    using CostType = typename Node::CostType;

    void add(Node *n) {
        buckets[n->f].push_back(n);
        n->setOLLocation(buckets[n->f].size() - 1);
        size_++;
    }

    std::size_t size() const { return size_; }

    bool empty() const {return size() == 0;}

    void update(Node *n, CostType newF) {
        if (FType(newF) == FType(n->f)) {
            //std::cout << "Nothing needs to be done in update" << std::endl;
            return;
        }
        erase(n->f, n->olLocation());
        n->f = newF;
        add(n);
    }

    Node *deleteMin() {
        return erase(buckets.begin()->first);
    }

    // the smallest possible f-value in the current bucket
    FType curF() { return buckets.begin()->first; }

    void dump() const {
        for (auto b : buckets) {
            std::cout << b.first << ": ";
            for (auto n: b.second)
                std::cout << n->f << "(" << n->olLocation() << ") ";
            std::cout << std::endl;
        }
    }

private:
    std::map<FType, std::vector<Node *>> buckets;
    int size_ = 0;
    Node *erase(FType bucketNum) {
        return erase(bucketNum, buckets[bucketNum].size()-1);
    }

    Node *erase(FType bucketNum, OLLocation pos) {
        auto &bucket = buckets[bucketNum];
        auto res = bucket[pos];

        // copy the last node of the bucket here
        bucket[pos] = bucket.back();
        bucket[pos]->setOLLocation(pos);

        // remove the last node of the bucket
        bucket.pop_back();
        if (bucket.empty()) buckets.erase(bucketNum);

        // the client code wants to take care of it
        // remember to re-institute this line in the generic library B"H
        // res->heapIndex = -1;
        size_--;
        return res;
    }
};

#endif
