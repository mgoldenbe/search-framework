#ifndef ASTAR_H
#define ASTAR_H

// Discontinued the use of template template parameters.
// http://stackoverflow.com/q/34130672/2725810
template <class Open = OL,
          template <class, class> class GoalHandler = GOAL_HANDLER,
          class Heuristic = HEURISTIC, class Graph = GRAPH,
          class AlgorithmLogger = LOGGER>
struct Astar {
    using Node = typename Open::Node;
    using NodeData = typename Node::NodeData;
    using CostType = typename Node::CostType;
    using NodeUniquePtr = typename Node::NodeUniquePtr;
    using State = typename Node::State;
    using MyInstance = Instance<State>;
    using Neighbor = typename State::Neighbor;

    static_assert(
        std::is_same<typename Node::StateSmartPtr,
                     StateSharedPtrT<State>>::value ||
            (std::is_same<Graph, NoGraph<State, CostType>>::value &&
             std::is_same<AlgorithmLogger, Nothing>::value),
        "In Astar: if a graph and/or logger is "
        "used, then the node has to "
        "store a *shared pointer*, not a "
        "*unique pointer* to state.");

    Astar(MyInstance &instance, Graph &graph, AlgorithmLogger &logger)
        : start_(instance.start()), // will change for startHandler
          goalHandler_(instance, logger), heuristic_(instance), graph_(graph),
          logger_(logger), oc_(), cur_(nullptr), children_() {}

    CostType run() {
        TimerLock lock{time_}; (void)lock;
        NodeUniquePtr startNode(new Node(start_));
        startNode->f = heuristic_(startNode.get());
        graph_.add(startNode->shareState());
        log<Events::MarkedStart>(logger_, startNode.get());
        goalHandler_.template logInit<Node>();
        oc_.add(startNode);
        while (!oc_.empty() && !goalHandler_.done()) {
            cur_ = oc_.minNode();
            log<Events::Selected>(logger_, cur_);
            onSelectAndExpand(std::integral_constant<
                bool, std::is_same<decltype(goalHandler_.onSelect(cur_, res_)),
                                   bool>::value>());
        }
        if (oc_.empty()) return CostType{-1};
        return res_;
    }

    MeasureSet measures() const {
        return {expanded_, denied_, generated_, time_, cost_};
    }
private:
    State start_; // We should consider making this local
    GoalHandler<State, AlgorithmLogger> goalHandler_;
    Heuristic heuristic_;
    Graph &graph_;
    AlgorithmLogger &logger_;
    OCL<Open> oc_;

    // We should consider making these local
    Node *cur_;
    std::vector<Neighbor> children_;
    CostType res_{-1};

    // Stats
    Measure expanded_{"Expanded"};
    Measure denied_{"Denied"};
    Measure generated_{"Generated"};
    Timer time_{"Time (ms.)"};
    Measure cost_{"Cost"};

    void onSelectAndExpand(std::true_type) {
        bool expandFlag = goalHandler_.onSelect(cur_, res_);
        cost_.set(res_);
        if (!expandFlag) {
            // The following code will need to become more generic
            auto oldCost = cur_->f;
            cur_->f = cur_->g + heuristic_(cur_);
            if (cur_->f > oldCost) {
                // Need to give info about the change of node information!
                ++denied_;
                log<Events::DeniedExpansion>(logger_, cur_);
                oc_.reInsert(cur_);
                return;
            } else
                log<Events::ResumedExpansion>(logger_, cur_);
        }
        expand();
    }

    void onSelectAndExpand(std::false_type) {
        goalHandler_.onSelect(cur_, res_);
        cost_.set(res_);
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
        log<Events::Closed>(logger_, cur_);
    }

    void handleChild(StateUniquePtrT<State> &child, CostType cost) {
        CostType myG = cur_->g + cost;
        ++generated_;
        auto childNode = oc_.getNode(*child);
        if (childNode) {
            graph_.add(cur_->shareState(), childNode->shareState(), cost);
            if (myG < childNode->g) {
                auto oldPriority = typename Open::Priority(childNode);
                CostType improvement = childNode->g - myG;
                childNode->g = myG;
                childNode->f -= improvement;
                childNode->setParent(cur_);
                oc_.update(childNode, oldPriority);
                log<Events::Generated>(logger_, childNode);
                log<Events::EnteredOpen>(logger_, childNode);
            }
            return;
        }
        NodeUniquePtr newNode(new Node(child));
        newNode->g = myG;
        newNode->f = myG + heuristic_(newNode.get());
        newNode->setParent(cur_);
        graph_.add(cur_->shareState(), newNode->shareState(), cost);
        log<Events::Generated>(logger_, newNode.get());
        log<Events::EnteredOpen>(logger_, newNode.get());
        oc_.add(newNode);
    }
};

#endif
