#ifndef OCL_H
#define OCL_H

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
template <class OpenList> struct OCL {
    using Node = typename OpenList::Node;
    using Priority = typename OpenList::Priority;
    using CostType = typename Node::CostType;
    using NodeUniquePtr = typename Node::NodeUniquePtr;
    using State = typename Node::State;

    void dump() const {
        std::cout << "-----------------" << std::endl;
        std::cout << "Dumping OCL:" << std::endl;
        std::cout << "-----------------" << std::endl;
        std::cout << "SLB_OL: " << ol.size() << " elements" << std::endl;
        ol.dump();
        std::cout << "-----------------" << std::endl;
        std::cout << "CL: " << hash.size() << " elements" << std::endl;
        for (auto it = hash.begin(); it != hash.end(); ++it)
            std::cout << *(it->second) << std::endl;
        std::cout << "-----------------" << std::endl << std::endl;
    }

    bool empty() const { return ol.empty(); }

    // Returning a non-const pointer is unsafe, but I do not have a better
    // solution for now. The user needs to be able to update g and f, to call
    // update() and to call close(). The danger is that he can forget to call
    // update().
    Node *getNode(const State &s) {
        auto it = hash.find(s);
        if (it == hash.end()) return nullptr;
        return (it->second).get();
    }

    void update(Node *n, const Priority &oldPriority) {
        ol.update(n, oldPriority);
    }

    void add(NodeUniquePtr &n) {
        ol.add(n.get());
        auto key = n->state();
        hash[key] = std::move(n);
    }

    void reInsert(Node *n) {
        ol.add(n);
    }

    Node *minNode() { return ol.deleteMin(); }

    void close(Node *n) { n->setBucketPosition(-1); }

private:
    OpenList ol;
    std::unordered_map<State, NodeUniquePtr, StateHash<State>>
    hash; // State needs to implement moving
          // copy constructor for insertions
          // to be effecient.
};

#endif
