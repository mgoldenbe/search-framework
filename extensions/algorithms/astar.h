#ifndef ASTAR_H
#define ASTAR_H

/// \file
/// \brief The \ref Astar class.
/// \author Meir Goldenberg

#include "algorithm.h"

template <ALG_TPARAMS_NO_DEFAULTS, class Open>
struct Astar; // Forward declare for the following type traits to work.

template <ALG_TPARAMS_NO_DEFAULTS, class Open>
struct AlgorithmTraits<Astar<ALG_TARGS, Open>> {
    BASE_TRAITS_TYPES
    using OC = OpenClosedList<Open>; ///< The open and closed lists type.
};

/// The \c A* search algorithm.
/// \tparam logFlag If \c true, the events generated by the search algorithm are
/// logged. Otherwise, they are not.
/// \tparam Node_ The search node type.
/// \tparam GoalHandler The policy for handling goal conditions.
/// \tparam Heuristic The heuristic used by the search algorithm.
/// \tparam Open The open list type.
/// \note See the documentation of \ref ALG_TYPES and \ref ALG_DATA.
template <ALG_TPARAMS, class Open = SLB_OL>
struct Astar : Algorithm<Astar<ALG_TARGS, Open>, ALG_TARGS> {
    using MyType = Astar; ///< The type of Astar.
    using DirectBase = Algorithm<MyType, ALG_TARGS>;
    using GoalHandler = typename DirectBase::GoalHandler;
    using Heuristic = typename DirectBase::Heuristic;

    BASE_TRAITS_TYPES
    using OC = typename AlgorithmTraits<Astar>::OC;

    ALG_DATA

    /// Initializes the algorithm based on the problem instance.
    /// \param instance The problem instance.
    Astar(MyInstance &instance)
        : Base(instance), oc_(), cur_(nullptr), children_() {}

    /// Runs the algorithm.
    /// \return The solution cost, which is the average of solution costs among
    /// the goals. If there is no solution, then \c CostType{-1} is returned.
    CostType run() {
        TimerLock lock{time_}; (void)lock;
        NodeUniquePtr startNode(new Node(start_));
        startNode->f = heuristic_(startNode.get());
        log<Events::MarkedStart>(log_, startNode.get());
        goalHandler_.logInit();
        oc_.add(startNode);
        while (!oc_.empty() && !goalHandler_.done()) {
            cur_ = oc_.minNode();
            log<Events::Selected>(log_, cur_);
            handleSelected();
        }
        if (oc_.empty()) res_ = -1;
        cost_.set(res_);
        return res_;
    }

    /// Returns the statistics about the search algorithm's
    /// performance for solving the particular instance.
    /// \return The statistics about the search algorithm's
    /// performance for solving the particular instance.
    MeasureSet measures() const {
        return Base::measures().append(MeasureSet{denied_});
    }

    /// Computes the state graph based on the closed list.
    /// \return The state graph computed based on the closed list.
    StateGraph<State> graph() const {
        StateGraph<State> res;
        for (const auto &el: oc_.hash()) {
            auto &from = (el.second)->shareState();
            res.add(from);
            for (auto &n : from->successors()) {
                // `add` cares for duplicates
                auto myNeighbor = unique2shared(n.state());
                res.add(from, myNeighbor, n.cost());
            }
        }
        return res;
    }

    /// \name Services for policies.
    /// @{

    OC &oc() { return oc_; }
    Node *cur() { return cur_; }
    CostType &res() { return res_; }
    Measure &denied() { return denied_; }

    /// @}
private:
    OC oc_; ///< The open and closed lists.

    Node *cur_; ///< The currently selected node.

    /// The children of the currently selected node.
    std::vector<Neighbor> children_;

    CostType res_{-1}; ///< The solution cost.

    // Stats
    Measure denied_{"Denied"}; ///< The number of denied expansions.

    /// Handles the selected node.
    void handleSelected() {
        // Tag dispatch to call the correct version depending on whether the
        // goal handler is dealing with the issue of suspending expansions.
        // In the latter case, onSelect returns bool, otherwise it returns
        // void.
        handleSelected(
            std::integral_constant<bool, onSelectReturns<GoalHandler>()>());
    }

    /// Handles the selected node in the case that suspensions are to be dealt
    /// with.
    void handleSelected(std::true_type) {
        if (goalHandler_.onSelect()) expand();
    }

    /// Handles the selected node in the case that suspensions do not need to be
    /// dealt with.
    void handleSelected(std::false_type) {
        goalHandler_.onSelect();
        expand();
    }

    /// Expands the selected node.
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

    /// Handles the generated child.
    /// \param child The selected child.
    /// \param cost The cost of the action that led to the child.
    void handleChild(StateUniquePtrT<State> &child, CostType cost) {
        CostType myG = cur_->g + cost;
        ++generated_;
        auto childNode = oc_.getNode(*child);
        if (childNode) {
            if (myG < childNode->g) {
                log<Events::NotParent>(log_, childNode);
                auto oldPriority = typename Open::KeyType(childNode);
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
