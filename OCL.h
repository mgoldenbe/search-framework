#ifndef OCL_FILE
#define OCL_FILE

#include <unordered_map>

template <class State>
struct StateHash {
    std::size_t operator()(const State &s) const {
        return s.hash();
    }
};

template <class OL> struct OCL {
    using Node = typename OL::Node;
    using Priority = typename OL::Priority;
    using CostType = typename Node::CostType;
    using NodeUP = typename Node::NodeUP;
    using State = typename Node::State;

    void dump() const {
        std::cout << "-----------------" << std::endl;
        std::cout << "Dumping OCL:" << std::endl;
        std::cout << "-----------------" << std::endl;
        std::cout << "OL: " << ol.size() << " elements" << std::endl;
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

    void add(NodeUP &&n) {
        ol.add(n.get());
        auto key = n->state();
        hash[key] = std::move(n);
    }

    Node *minNode() { return ol.deleteMin(); }

    void close(Node *n) { n->setBucketPosition(-1); }

private:
    OL ol;
    std::unordered_map<State, NodeUP, StateHash<State>>
    hash; // State needs to implement moving
          // copy constructor for insertions
          // to be effecient.
};

#endif
