#ifndef ASTAR
#define ASTAR

#include <vector>
#include <type_traits>
#include <memory>
#include "Graph.h"
#include "AlgorithmLogger.h"
#include "AstarEvent.h"

// Discontinued the use of template template parameters.
// http://stackoverflow.com/q/34130672/2725810
template <class Open = OL, class MyInstance = INSTANCE,
          template <class, class> class GoalHandler = GOAL_HANDLER,
          class Heuristic = HEURISTIC, class Graph = GRAPH,
          class AlgorithmLogger = LOGGER>
struct Astar {
    using Node = typename Open::Node;
    using NodeData = typename Node::NodeData;
    using CostType = typename Node::CostType;
    using NodeUniquePtr = typename Node::NodeUniquePtr;
    using State = typename Node::State;
    using Neighbor = typename State::Neighbor;
    using Event = typename AlgorithmLogger::AlgorithmEvent;

    static_assert(
        std::is_same<typename Node::StateSmartPtr,
                     StateSharedPtrT<State>>::value ||
            (std::is_same<Graph, NoGraph<State, CostType>>::value &&
             std::is_same<AlgorithmLogger, NoAlgorithmLogger<Event>>::value),
        "In Astar: if a graph and/or logger is used, then the node has to "
        "store a *shared pointer*, not a *unique pointer* to state.");

    Astar(MyInstance &instance, Graph &graph, AlgorithmLogger &logger)
        : start_(instance.MyInstance::Start::state_),
          goalHandler_(instance, logger), heuristic_(instance), graph_(graph),
          logger_(logger), oc_(), cur_(nullptr), children_() {}

    CostType run() {
        TimerLock lock{time_}; (void)lock;
        NodeUniquePtr startNode(new Node(start_));
        startNode->f = heuristic_(startNode.get());
        graph_.add(startNode->shareState());
        logger_.log(Event(logger_, startNode.get(), Event::EventType::ROLE,
                          Event::StateRole::START));
        goalHandler_.template logInit<Node>();
        oc_.add(startNode);
        while (!oc_.empty() && !goalHandler_.done()) {
            cur_ = oc_.minNode();
            logger_.log(Event(logger_, cur_, Event::EventType::SELECTED));
            onSelectAndExpand(std::integral_constant<
                bool, std::is_same<decltype(goalHandler_.onSelect(cur_)),
                                   bool>::value>());
        }
        if (oc_.empty()) return CostType{-1};
        return cur_->f;
    }

    Stats stats() const {
        return {expanded_, generated_, time_};
    }
private:
    State start_; // We should consider making this local
    GoalHandler<MyInstance, AlgorithmLogger> goalHandler_;
    Heuristic heuristic_;
    Graph &graph_;
    AlgorithmLogger &logger_;
    OCL<Open> oc_;

    // We should consider making these local
    Node *cur_;
    std::vector<Neighbor> children_;

    // Stats
    Counter expanded_{"Expanded"};
    Counter generated_{"Generated"};
    Timer time_{"Time (microsec.)"};

    void onSelectAndExpand(std::true_type) {
        if (!goalHandler_.onSelect(cur_)) {
            // The following code will need to become more generic
            auto oldCost = cur_->f;
            cur_->f = cur_->g + heuristic_(cur_);
            if (cur_->f > oldCost) {
                // Need to give info about the change of node information!
                logger_.log(
                    Event(logger_, cur_, Event::EventType::DENIED_EXPANSION));
                oc_.reInsert(cur_);
                return;
            } else
                logger_.log(
                    Event(logger_, cur_, Event::EventType::RESUMED_EXPANSION));
        }
        expand();
    }

    void onSelectAndExpand(std::false_type) {
        goalHandler_.onSelect(cur_);
        expand();
    }

    void expand() {
        if (goalHandler_.done()) {
            // std::cout << "Done!" << std::endl;
            return;
        }
        ++expanded_;
        children_ = (cur_->state()).successors();
        for (auto &child : children_) {
            handleChild(child.state(), child.cost());
        }
        logger_.log(Event(logger_, cur_, Event::EventType::CLOSED));
    }

    void handleChild(StateUniquePtrT<State> &child, CostType cost) {
        ++generated_;
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
        newNode->f = newNode->g + heuristic_(newNode.get());
        newNode->setParent(cur_);
        graph_.add(cur_->shareState(), newNode->shareState(), cost);
        logger_.log(
            Event(logger_, newNode.get(), Event::EventType::BEGIN_GENERATE));
        logger_.log( // needed so currently generated node changes color
                     // cannot be after adding to OL, since std::move is
                     // performed there!
            Event(logger_, newNode.get(), Event::EventType::END_GENERATE));
        oc_.add(newNode);
    }
};

#endif
