#ifndef ASTAR
#define ASTAR

#include <vector>
#include "Graph.h"
#include <type_traits>

template <class State>
struct NoGoalHandler {
    NoGoalHandler(const State &goal) {(void)goal;}
    void update(const State &s) {(void)s;}
    bool done() const {return false;}
};

template <class State>
struct SingleGoalHandler {
    SingleGoalHandler(const State &goal) : goal_(goal) {}
    void update(const State &s) {
        if (s == goal_) done_ = true;
    }
    bool done() const {return done_;}
private:
    State goal_;
    bool done_ = false;
};

// Heuristic should have a static compute() member function
template <class Open, class Heuristic,
          template <class State> class GoalHandler = SingleGoalHandler,
          template <class State, class CostType> class Graph = NoGraph>
struct Astar {
    using Node = typename Open::Node;
    using CostType = typename Node::CostType;
    using NodeUniquePtr = typename Node::NodeUniquePtr;
    using State = typename Node::State;
    using Neighbor = typename State::Neighbor;

    static_assert(std::is_same<typename Node::StateSmartPtr,
                               StateSharedPtrT<State>>::value ||
                      std::is_same<Graph<State, CostType>,
                                   NoGraph<State, CostType>>::value,
                  "In Astar: if a graph is used, then the node has to store a "
                  "*shared pointer*, not a *unique pointer* to state.");

    Astar(const State &start, const GoalHandler<State> &goalHandler,
          Graph<State, CostType> &graph)
        : start_(start), goalHandler_(goalHandler), graph_(graph), oc_(),
          cur_(nullptr), children_() {}

    void run() {
        NodeUniquePtr startNode(new Node(start_));
        graph_.add(startNode->shareState());
        oc_.add(startNode);
        while (!oc_.empty() && !goalHandler_.done()) expand();
    }

    void expand() {
        cur_ = oc_.minNode();
        std::cout << "Selected: " << *cur_ << std::endl;
        goalHandler_.update(cur_->state());
        if (goalHandler_.done()) {
            //std::cout << "Done!" << std::endl;
            return;
        }
        children_ = (cur_->state()).successors();
        for (auto &child : children_) {
            handleChild(child.state(), child.cost());
        }
    }

    void handleChild(StateUniquePtrT<State> &child, CostType cost) {
        auto childNode = oc_.getNode(*child);
        if (childNode) {
            graph_.add(cur_->shareState(), childNode->shareState(), cost);
            return;
        }
        NodeUniquePtr newNode(new Node(child)); newNode->g = cur_->g + cost;
        graph_.add(cur_->shareState(), newNode->shareState(), cost);
        //std::cout << "    Generated: " << *newNode << std::endl;
        oc_.add(newNode);
    }

private:
    State start_;
    GoalHandler<State> goalHandler_;
    Graph<State, CostType> &graph_;
    OCL<Open> oc_;
    Node *cur_;
    std::vector<Neighbor> children_;
};

#endif
