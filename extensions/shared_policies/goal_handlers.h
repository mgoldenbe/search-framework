#ifndef SLB_EXTENSIONS_SHARED_POLICIES_GOAL_HANDLERS_H
#define SLB_EXTENSIONS_SHARED_POLICIES_GOAL_HANDLERS_H

/// \file
/// \brief Policies for handling conditions related to goal states.
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace policy {

/// \namespace slb::ext::policy::goalHandler
/// Policies for handling conditions related to goal states.
namespace goalHandler {

/// Checks at compile-time whether the given goal handler's \c onSelect member
/// function has a return value.
/// \tparam Handler The goal handler policy.
/// \return \c true if given goal handler's \c onSelect member
/// function has a return value and \c false otherwise.
template <class Handler>
constexpr bool onSelectReturns() {
    return std::is_same<decltype(std::declval<Handler>().onSelect()),
                        bool>::value;
}

/// Handling conditions related to goal states for the case when the set of the
/// goal states is empty.
/// \tparam Algorithm The search algorithm.
template <class MyAlgorithm> // for uniformity
struct NoGoal {
    POLICY_TYPES

    /// Initializes the policy based on a problem instance and a log of events
    /// generated by the search algorithm.
    NoGoal(MyAlgorithm &) {}

    /// Handles selection of a node by the search algorithm.
    void onSelect() {}

    /// Returns \c true if all the goals were found with the required quality
    /// and the search algorithm should terminate. Returns \c false otherwise.
    /// \return \c true if all the goals were found with the required quality
    /// and \c false otherwise.
    bool done() const {return false;}

    /// Passes the goals to the log of the search algorithm. In this particular
    /// case, there are no goals to pass.
    /// \tparam Node The search node type.
    void logInit() {}
};

/// Handling conditions related to goal states for the case when there is a
/// single goal state. Assumes that, at goal, f=g holds.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
struct SingleGoal {
    POLICY_TYPES

    /// The constructor.
    /// \param alg Reference to the search algorithm.
    SingleGoal(MyAlgorithm &alg)
        : alg_(alg), log_(alg.log()), goal_(alg.instance().goal()) {}

    /// Handles selection of a node by the search algorithm.
    void onSelect() {
        Node *n = alg_.cur();
        if (n->f == n->g)
            // the above check might be much faster than comparing the states
            if (n->state() == goal_) doneGoal(n);
    }

    /// Returns \c true if all the goals were found with the required quality
    /// and the search algorithm should terminate. Returns \c false otherwise.
    /// \return \c true if all the goals were found with the required quality
    /// and \c false otherwise.
    bool done() const {return done_;}

    /// Passes the goals to the log of the search algorithm. In this particular
    /// case, there is only a single goal to pass.
    void logInit() {
        log<ext::event::MarkedGoal>(log_, std::make_shared<Node>(goal_).get());
    }
private:
    /// Reference to the search algorithm.
    MyAlgorithm &alg_;

    /// The log of events generated by the search algorithm.
    Log &log_;

    /// The goal state.
    State goal_;

    /// Flag showing whether the search algorithm has accomplished its mission
    /// and should terminate.
    bool done_ = false;

    /// Handler the search node that is a goal.
    /// \param n The goal search node.
    virtual void doneGoal(Node *n) {
        log<ext::event::SolvedGoal>(log_, n);
        done_ = true;
        alg_.res() = n->g;
    }
};

/// Handling conditions related to goal states for the case when there are
/// multiple goal states.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
struct MultipleGoal {
    POLICY_TYPES

    /// The type for a goal that has not been solved with the required quality.
    using ActiveGoal = std::pair<State, int>;

    /// Iterator over active goal states.
    using ActiveGoalsIt = typename std::vector<ActiveGoal>::iterator;

    /// Initializes the policy based on a problem instance and a log of events
    /// generated by the search algorithm.
    /// \param alg Reference to the search algorithm.
    MultipleGoal(MyAlgorithm &alg) : alg_(alg), log_(alg.log()) {
        auto const &goals = alg.instance().goals();
        int i = -1;
        for (const auto &g: goals)
            activeGoals_.push_back(std::make_pair(g, ++i));
    }

    /// Handles selection of a node by the search algorithm.
    void onSelect() {
        Node *n = alg_.cur();
        auto it = findActive(n->state());
        if (it != activeGoals_.end()) doneGoal(n, it);
    }

    /// Returns \c true if all the goals were found with the required quality
    /// and the search algorithm should terminate. Returns \c false otherwise.
    /// \return \c true if all the goals were found with the required quality
    /// and \c false otherwise.
    bool done() const {return done_;}

    /// Passes the goals to the log of the search algorithm.
    /// \tparam Node The search node type.
    void logInit() {
        for (auto &g : activeGoals_)
            log<ext::event::MarkedGoal>(log_,
                                    std::make_shared<Node>(g.first).get());
    }

    /// Returns the active goals.
    /// \return Const reference to the vector of active goals.
    const std::vector<ActiveGoal> &activeGoals() { return activeGoals_; }

protected:
    /// Reference to the search algorithm.
    MyAlgorithm &alg_;

    /// The log of events generated by the search algorithm.
    Log &log_;

    /// The active goal states.
    std::vector<ActiveGoal> activeGoals_;

    /// Flag showing whether the search algorithm has accomplished its mission
    /// and should terminate.
    bool done_ = false;

    /// Determines whether the given goal is active. Optionally removes the goal
    /// from the list of active goals.
    /// \tparam eraseFlag If \c true and the goal is active the goal is removed
    /// from the list of active goals.
    /// \param goal The goal state of interest.
    /// \return \c true if \c goal is an active goal and \c false otherwise.
    template <bool eraseFlag = false>
    bool isActive(const State &goal) {
        auto it = findActive(goal);
        if (it == activeGoals_.end()) return false;
        if (eraseFlag) activeGoals_.erase(it);
        return true;
    }

    /// Computes the ID of the given active goal state.
    /// \param goal The active goal state of interest.
    /// \return ID of the active goal state \c goal.
    int activeGoalID(const State &goal) {
        auto it = findActive(goal);
        assert(it != activeGoals_.end());
        return it->second;
    }

    /// Computes the iterator to the given active goal state.
    /// \param goal The active goal state of interest.
    /// \return The iterator to the active goal state \c goal.
    ActiveGoalsIt findActive(const State &goal) {
        return std::find_if(
            activeGoals_.begin(), activeGoals_.end(),
            [goal](const ActiveGoal &a) { return a.first == goal; });
    }

    /// Handles the search node that is a goal.
    /// \param n The goal search node.
    /// \param it The iterator to the goal state.
    virtual void doneGoal(Node *n, ActiveGoalsIt it) {
        log<ext::event::SolvedGoal>(log_, n);
        log<ext::event::HideLast>(log_, n);
        auto &res = alg_.res();
        activeGoals_.erase(it);
        int nDoneGoals = alg_.instance().goals().size() - activeGoals_.size();
        res = (res * (nDoneGoals - 1) + n->g) / nDoneGoals;
        // res += (res == -1 ? 1 : 0) + n->g;
        if (activeGoals_.empty()) done_ = true;
    }
};

/// Handling conditions related to goal states for the case when there are
/// multiple goal states and a heuristic that stores the goal state responsible
/// for the heuristic value is used.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
struct LazyUpdate: MultipleGoal<MyAlgorithm> {
    POLICY_TYPES
    /// The direct base.
    using DirectBase = MultipleGoal<MyAlgorithm>;

    /// Using the direct base's constructors.
    using DirectBase::DirectBase;

    using DirectBase::alg_;
    using DirectBase::log_;
    using DirectBase::activeGoals_;
    using DirectBase::done_;
    using DirectBase::isActive;

    /// Handles selection of a node by the search algorithm. If the goal
    /// responsible for the heuristic value of the node had been found with the
    /// required quality, the heuristic is re-computed based on the remaining
    /// goals. If the node's cost grows based on that check, then \c false is
    /// returned indicating the denial of expansion of this node.
    /// \return \c true if the currently selected node should be expanded and \c
    /// false otherwise.
    bool onSelect() {
        Node *n = alg_.cur();

        // Check identity of goal resposible for heuristic
        if (!isActive(n->responsibleGoal)) {
            log<ext::event::SuspendedExpansion>(log_, n);
            return !isDenied(n);
        }

        auto it = this->findActive(n->state());
        if (it != activeGoals_.end()) {
            this->doneGoal(n, it);
            if (!done_) update(n);
            return false;
        }

        return true;
    }

protected:
    /// Determines whether the goal with the given index into the array of goal
    /// states is active. Optionally removes the goal from the list of active
    /// goals.
    /// \tparam eraseFlag If \c true and the goal is active the goal is removed
    /// from the list of active goals.
    /// \param goalId The index into the array of goal states for the goal state
    /// of interest.
    /// \return \c true if \c goal is an active goal and \c false otherwise.
    template <bool eraseFlag = false>
    bool isActive(int goalId) {
        for (auto it = activeGoals_.begin(); it != activeGoals_.end(); ++it)
            if (it->second == goalId) {
                if (eraseFlag)
                    activeGoals_.erase(it);
                return true;
            }
        return false;
    }

    /// Determines whether the given search node should be denied expansion.
    /// \param n The search node of interest.
    /// \return \c true if the given search node should be denied expansion and
    /// \c false otherwise.
    bool isDenied(Node *n) {
        auto oldCost = n->f;
        n->f = n->g + alg_.generator().heuristic(n);
        if (n->f > oldCost) {
            ++alg_.denied();
            log<ext::event::DeniedExpansion>(log_, n);
            alg_.oc().reInsert(n);
            return true;
        }
        log<ext::event::ResumedExpansion>(log_, n);
        return false;
    }

    /// Update the information associated with the given search node.
    /// \param n The search node.
    void update(Node *n) {
        n->updateH(alg_.generator().heuristic(n));
        alg_.oc().reInsert(n);
    }
};

#ifndef SLB_PARTIAL_UPDATE_FLAG
/// The default value for the partial update flag for k-goal search. By default,
/// the update of the open list is total.
#define SLB_PARTIAL_UPDATE_FLAG false
#endif

/// Handling conditions related to goal states for the case when there are
/// multiple goal states and the whole open list must be re-computed whenever
/// a goal is solved with the required quality.
/// \tparam MyAlgorithm The search algorithm.
/// \tparam partialRecomputeFlag \c true if partial (rather than total)
/// re-computation of the open list is to be performed.
template <class MyAlgorithm, bool partialUpdateFlag = SLB_PARTIAL_UPDATE_FLAG>
struct TotalUpdateT: MultipleGoal<MyAlgorithm> {
    POLICY_TYPES
    /// The direct base.
    using DirectBase = MultipleGoal<MyAlgorithm>;

    /// Using the direct base's constructors.
    using DirectBase::DirectBase;

    using DirectBase::alg_;
    using DirectBase::done_;
    using DirectBase::activeGoals_;

    /// Iterator over active goal states.
    using ActiveGoalsIt = typename DirectBase::ActiveGoalsIt;

    /// Returns the time stamp of the last found goal. This is also when the
    /// open list was re-computed last.
    /// \return /// The time stamp when the last goal was found.
    int stamp() { return stamp_; }
protected:
    /// Time stamp when the last goal was found.
    int stamp_;

    /// Handles the found goal. Recomputes the open list and saves the time
    /// stamp.
    /// \param n The goal node.
    /// \param it The iterator to the active goal state.
    virtual void doneGoal(Node *n, ActiveGoalsIt it) {
        if (false) { // Output the average number of nodes in open for debugging
            static std::ofstream debug{"debug/ol_debug.txt"};
            static int nInstances = 100;
            static int nGoals = activeGoals_.size();
            static int curGoal = 0;
            static int curInstance = 0;
            static std::vector<int> sums(nGoals);
            sums[curGoal] += alg_.oc().ol().size();
            if (curInstance == nInstances - 1)
                debug << (double)sums[curGoal] / nInstances << std::endl;
            curGoal = (curGoal + 1) % nGoals;
            if (curGoal == 0) ++curInstance;
        }
        auto goalID = it->second; // must be before the call to doneGoal.
        DirectBase::doneGoal(n, it);
        if (!activeGoals_.empty()) {
            // Not the last goal.
            if (partialUpdateFlag)
                alg_.partialRecomputeOpen([goalID](Node *cur) {
                    return cur->responsibleGoal == goalID;
                });
            else
                alg_.recomputeOpen();
        }
        stamp_ = alg_.stamp();
    }
};
/// The total update policy.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm> using TotalUpdate = TotalUpdateT<MyAlgorithm>;

} // namespace
} // namespace
} // namespace
} // namespace

#endif
