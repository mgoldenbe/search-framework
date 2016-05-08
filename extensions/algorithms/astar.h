#ifndef ASTAR_H
#define ASTAR_H

// Discontinued the use of template template parameters.
// http://stackoverflow.com/q/34130672/2725810
template <class Open = SLB_OL,
          template <class, class> class GoalHandler = SLB_GOAL_HANDLER,
          class Heuristic = SLB_HEURISTIC, class AlgorithmLog = Nothing>
struct Astar {
    using Node = typename Open::Node;
    using NodeData = typename Node::NodeData;
    using CostType = typename Node::CostType;
    using NodeUniquePtr = typename Node::NodeUniquePtr;
    using State = typename Node::State;
    using MyInstance = Instance<State>;
    using Neighbor = typename State::Neighbor;

    Astar(MyInstance &instance, AlgorithmLog &log)
        : start_(instance.start()), // will change for startHandler
          goalHandler_(instance, log), heuristic_(instance), log_(log), oc_(),
          cur_(nullptr), children_() {}

    CostType run() {
        TimerLock lock{time_}; (void)lock;
        NodeUniquePtr startNode(new Node(start_));
        startNode->f = heuristic_(startNode.get());
        log<Events::MarkedStart>(log_, startNode.get());
        goalHandler_.template logInit<Node>();
        oc_.add(startNode);
        while (!oc_.empty() && !goalHandler_.done()) {
            cur_ = oc_.minNode();
            log<Events::Selected>(log_, cur_);
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

    StateGraph<State> graph() const {
        StateGraph<State> res;
        for (const auto &el: oc_.hash()) {
            auto &from = (el.second)->shareState();
            res.add(from);
            for (auto &n : from->successors())
                // `add` cares for duplicates
                res.add(from, unique2shared(n.state()), n.cost());
        }
        return res;
    }
private:
    State start_; // We should consider making this local
    GoalHandler<State, AlgorithmLog> goalHandler_;
    Heuristic heuristic_;
    AlgorithmLog &log_;
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
                log<Events::DeniedExpansion>(log_, cur_);
                oc_.reInsert(cur_);
                return;
            } else
                log<Events::ResumedExpansion>(log_, cur_);
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
        log<Events::Closed>(log_, cur_);
    }

    void handleChild(StateUniquePtrT<State> &child, CostType cost) {
        CostType myG = cur_->g + cost;
        ++generated_;
        auto childNode = oc_.getNode(*child);
        if (childNode) {
            if (myG < childNode->g) {
                auto oldPriority = typename Open::Priority(childNode);
                CostType improvement = childNode->g - myG;
                childNode->g = myG;
                childNode->f -= improvement;
                childNode->setParent(cur_);
                oc_.update(childNode, oldPriority);
                log<Events::Generated>(log_, childNode);
                log<Events::EnteredOpen>(log_, childNode);
            }
            else {
                log<Events::NotGenerated>(log_, childNode, cur_);
                log<Events::HideLast>(log_, childNode);
            }

            return;
        }
        NodeUniquePtr newNode(new Node(child));
        newNode->g = myG;
        newNode->f = myG + heuristic_(newNode.get());
        newNode->setParent(cur_);
        log<Events::Generated>(log_, newNode.get());
        log<Events::EnteredOpen>(log_, newNode.get());
        oc_.add(newNode);
    }
};

template <class Open = SLB_OL,
          template <class, class> class GoalHandler = SLB_GOAL_HANDLER,
          class Heuristic = SLB_HEURISTIC>
using LoggingAstar =
    Astar<Open, GoalHandler, Heuristic, AlgorithmLog<typename Open::Node>>;

#endif
