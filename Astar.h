#ifndef ASTAR
#define ASTAR

#include <vector>
#include "Graph.h"

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
          template <class StateNeighbor> class Graph = NoGraph>
struct Astar {
    using Node = typename Open::Node;
    using CostType = typename Node::CostType;
    using NodeUP = typename Node::NodeUP;
    using State = typename Node::State;
    using StateUP = typename Node::StateUP;
    using Neighbor = typename State::Neighbor;

    Astar(const State &start, const GoalHandler<State> &goalHandler, Graph<Neighbor> &graph)
        : start_(start), goalHandler_(goalHandler), graph_(graph), oc_(),
          cur_(nullptr), children_() {
        graph_.add(start); // When Graph is instantiated to be NoGraph, the
                           // binary should be the same with or without this
                           // line!
    }

    void run() {
        NodeUP startNode(new Node(start_));
        oc_.add(startNode);
        while (!oc_.empty() && !goalHandler_.done()) expand();
    }

    void expand() {
        cur_ = oc_.minNode();
        std::cout << "Selected: " << *cur_ << std::endl;
        goalHandler_.update(cur_->state());
        if (goalHandler_.done()) {
            std::cout << "Done!" << std::endl;
            return;
        }
        children_ = (cur_->state()).successors();
        for (auto &child : children_) {
            graph_.add(cur_->state(), child);
            handleChild(child.state(), cur_->g + child.cost());
        }
    }

    void handleChild(StateUP &child, CostType g) {
        auto childNode = oc_.getNode(*child);
        if (childNode) return;
        NodeUP newNode(new Node(child)); newNode->g = g;
        std::cout << "    Generated: " << *newNode << std::endl;
        oc_.add(newNode);
    }

private:
    State start_;
    GoalHandler<State> goalHandler_;
    Graph<Neighbor> &graph_;
    OCL<Open> oc_;
    Node *cur_;
    std::vector<Neighbor> children_;
};

#endif
