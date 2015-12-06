#ifndef ASTAR
#define ASTAR

#include <vector>
#include <type_traits>
#include <memory>
#include "Graph.h"
#include "AlgorithmLogger.h"
#include "AstarEvent.h"

// Heuristic should have a static compute() member function
template <class Open, class Heuristic,
          template <class State> class GoalHandler = SingleGoalHandler,
          template <class State, class CostType> class Graph = NoGraph,
          class AlgorithmLogger = NoAlgorithmLogger<AstarEvent<
              typename Open::Node::State, typename Open::Node::NodeData>>>
struct Astar {
    using Node = typename Open::Node;
    using NodeData = typename Node::NodeData;
    using CostType = typename Node::CostType;
    using NodeUniquePtr = typename Node::NodeUniquePtr;
    using State = typename Node::State;
    using Neighbor = typename State::Neighbor;
    using Event = typename AlgorithmLogger::AlgorithmEvent;

    static_assert(std::is_same<typename Node::StateSmartPtr,
                               StateSharedPtrT<State>>::value ||
                      std::is_same<Graph<State, CostType>,
                                   NoGraph<State, CostType>>::value,
                  "In Astar: if a graph is used, then the node has to store a "
                  "*shared pointer*, not a *unique pointer* to state.");

    Astar(const State &start, const GoalHandler<State> &goalHandler,
          const Heuristic &heuristic, Graph<State, CostType> &graph,
          AlgorithmLogger &logger)
        : start_(start), goalHandler_(goalHandler), heuristic_(heuristic),
          graph_(graph), logger_(logger), oc_(), cur_(nullptr), children_() {}

    void run() {
        NodeUniquePtr startNode(new Node(start_));
        graph_.add(startNode->shareState());
        logger_.log(
            Event(startNode->shareState(), Event::StateRole::START, nullptr));
        goalHandler_.logInit(logger_);
        oc_.add(startNode);
        while (!oc_.empty() && !goalHandler_.done()) expand();
    }

    void expand() {
        cur_ = oc_.minNode();
        auto parent = cur_->parent() ? cur_->parent()->shareState() : nullptr;
        logger_.log(
            Event(cur_->shareState(), Event::EventType::SELECTED, parent));
        goalHandler_.update(cur_, logger_);
        if (goalHandler_.done()) {
            //std::cout << "Done!" << std::endl;
            return;
        }
        children_ = (cur_->state()).successors();
        for (auto &child : children_) {
            handleChild(child.state(), child.cost());
        }
        logger_.log(
            Event(cur_->shareState(), Event::EventType::CLOSED, parent));
    }

    void handleChild(StateUniquePtrT<State> &child, CostType cost) {
        auto childNode = oc_.getNode(*child);
        if (childNode) {
            graph_.add(cur_->shareState(), childNode->shareState(), cost);
            /*
            logger_.log(Event(childNode->shareState(),
                              Event::EventType::BEGIN_GENERATE,
                              cur_->shareState(), (NodeData) * childNode));
            logger_.log(Event(childNode->shareState(),
                              Event::EventType::END_GENERATE,
                              cur_->shareState(), (NodeData) * childNode));
            */
            return;
        }
        NodeUniquePtr newNode(new Node(child));
        newNode->g = cur_->g + cost;
        newNode->f = newNode->g + heuristic_(newNode->state());
        newNode->setParent(cur_);
        graph_.add(cur_->shareState(), newNode->shareState(), cost);
        logger_.log(Event(newNode->shareState(),
                          Event::EventType::BEGIN_GENERATE, cur_->shareState(),
                          (NodeData) * newNode));
        logger_.log( // needed so currently generated node changes color
            // cannot be after adding to OL, since std::move is
            // performed there!
            Event(newNode->shareState(), Event::EventType::END_GENERATE,
                  cur_->shareState(), (NodeData) * newNode));
        oc_.add(newNode);
    }

private:
    State start_;
    GoalHandler<State> goalHandler_;
    const Heuristic &heuristic_;
    Graph<State, CostType> &graph_;
    AlgorithmLogger &logger_;
    OCL<Open> oc_;
    Node *cur_;
    std::vector<Neighbor> children_;
};

#endif
