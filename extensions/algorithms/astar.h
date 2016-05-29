#ifndef ASTAR_H
#define ASTAR_H

#include "algorithm.h"

// Discontinued the use of template template parameters.
// http://stackoverflow.com/q/34130672/2725810
template <ALG_TPARAMS, class Open = SLB_OL>
struct Astar : Algorithm<ALG_TARGS> {
    ALG_TYPES
    ALG_DATA

    Astar(MyInstance &instance)
        : Base(instance), oc_(), cur_(nullptr), children_() {}

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
            // Tag dispatch to call the correct version depending on whether the
            // goal handler is dealing with the issue of suspending expansions.
            // In the latter case, onSelect returns bool, otherwise it returns
            // void.
            onSelectAndExpand(std::integral_constant<
                bool, std::is_same<decltype(goalHandler_.onSelect(cur_, res_)),
                                   bool>::value>());
        }
        cost_.set(res_);
        if (oc_.empty()) return CostType{-1};
        return res_;
    }

    MeasureSet measures() const {
        return Base::measures().append(MeasureSet{denied_});
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
    OpenClosedList<Open> oc_;

    // We should consider making these local
    Node *cur_;
    std::vector<Neighbor> children_;
    CostType res_{-1};

    // Stats
    Measure denied_{"Denied"};

    void onSelectAndExpand(std::true_type) {
        bool expandFlag = goalHandler_.onSelect(cur_, res_);
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
                log<Events::NotParent>(log_, childNode);
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
                log<Events::NothingToDo>(log_, childNode, cur_);
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

#endif
