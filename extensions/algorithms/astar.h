#ifndef ASTAR_H
#define ASTAR_H

/// \file
/// \brief The \ref Astar class.
/// \author Meir Goldenberg

#include "algorithm.h"

template <ALG_TPARAMS_NO_DEFAULTS, template <class> class Open>
struct Astar; // Forward declare for the following type traits to work.

template <ALG_TPARAMS_NO_DEFAULTS, template <class> class Open>
struct AlgorithmTraits<Astar<ALG_TARGS, Open>> {
    BASE_TRAITS_TYPES
    using MyAlgorithm = Astar<ALG_TARGS, Open>;
    /// The open and closed lists type.
    using OC = OpenClosedList<Open<MyAlgorithm>>;
    using Generator = Generator_<MyAlgorithm>;
    //using Neighbor = typename Generator::Neighbor;
};

/// The \c A* search algorithm.
/// \tparam logFlag If \c true, the events generated by the search algorithm are
/// logged. Otherwise, they are not.
/// \tparam Node_ The search node type.
/// \tparam GoalHandler The policy for handling goal conditions.
/// \tparam Heuristic The heuristic used by the search algorithm.
/// \tparam Open The open list type.
/// \note See the documentation of \ref ALG_TYPES and \ref ALG_DATA.
template <ALG_TPARAMS, template <class> class Open_ = SLB_OL>
struct Astar : Algorithm<Astar<ALG_TARGS, Open_>, ALG_TARGS> {
    BASE_TRAITS_TYPES
    using Open = Open_<Astar>;
    using OC = typename AlgorithmTraits<Astar>::OC;
    using Neighbor = typename AlgorithmTraits<Astar>::Generator::Neighbor;

    using DirectBase = Algorithm<Astar<ALG_TARGS, Open_>, ALG_TARGS>;
    using GoalHandler = typename DirectBase::GoalHandler;
    using InitialHeuristic = typename DirectBase::InitialHeuristic;
    using Generator = typename DirectBase::Generator;

    using MyType = Astar; ///< The type of Astar; required for \ref ALG_DATA symbol.
    ALG_DATA

    /// Initializes the algorithm based on the problem instance.
    /// \param instance The problem instance.
    Astar(const MyInstance &instance)
        : Base(instance), oc_(*this), cur_(nullptr), neighbors_() {}

    /// Runs the algorithm.
    /// \return The solution cost, which is the average of solution costs among
    /// the goals. If there is no solution, then \c CostType{-1} is returned.
    ReturnType run() {
        TimerLock lock{time_}; (void)lock;
        NodeUniquePtr startNode(new Node(start_));
        startNode->set(0, initialHeuristic_(startNode.get()), this->stamp());
        log<Events::MarkedStart>(log_, startNode.get());
        goalHandler_.logInit();
        oc_.add(startNode);
        while (!oc_.empty() && !goalHandler_.done()) {
            cur_ = oc_.minNode();
            log<Events::Selected>(log_, cur_);
            handleSelected();
        }
        if (!goalHandler_.done()) res_ = -1;
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
            auto from = make_deref_shared<const State>(el.second->state());
            res.add(from);
            for (auto &n : from->stateSuccessors()) {
                // `add` cares for duplicates
                res.add(from, make_deref_shared<const State>(n.state()),
                        n.cost());
            }
        }
        return res;
    }

    /// \name Services for policies.
    /// @{

    OC &oc() { return oc_; }
    Node *cur() { return cur_; }
    Measure &denied() { return denied_; }
    void recomputeOpen() { oc_.recomputeOpen(); }

    /// @}
private:
    OC oc_; ///< The open and closed lists.

    Node *cur_; ///< The currently selected node.

    /// The children of the currently selected node.
    std::vector<Neighbor> neighbors_;

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
        neighbors_ = generator_.successors(cur_->state());
        for (auto &n : neighbors_)
            handleNeighbor(n);
        log<Events::Closed>(log_, cur_);
    }

    /// Handles the current neighbor.
    /// \param n The selected child.
    /// \param cost The cost of the action that led to the child.
    void handleNeighbor(const Neighbor &n) {
        auto childState = generator_.state(n);
        CostType myG = cur_->g + n.cost();
        ++generated_;
        auto childNode = oc_.getNode(childState);
        if (childNode) {
            if (myG < childNode->g) {
                // only consistent case for now
                // assert(childNode->bucketPosition() >= 0);
                log<Events::NotParent>(log_, childNode);
                auto oldPriority = oc_.priority(childNode);
                childNode->updateG(myG);
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
        NodeUniquePtr newNode(new Node(childState));
	newNode->setParent(cur_);
        newNode->set(myG, generator_.heuristic(n, newNode.get()),
                     this->stamp());
        log<Events::Generated>(log_, newNode.get());
        log<Events::EnteredOpen>(log_, newNode.get());
        oc_.add(newNode);
    }
};

#endif
