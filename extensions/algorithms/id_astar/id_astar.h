#ifndef ID_ASTAR_H
#define ID_ASTAR_H

/// \file
/// \brief The \ref IdAstar class.
/// \author Meir Goldenberg

#include "../algorithm.h"
#include "backtrack.h"

// The symbols tool is currently not smart enough to take care of unused symbols
// that are templates.
template <class, bool> struct slb_tcb_nothing {};
#ifndef SLB_ID_ASTAR_BACKTRACK_LOCK
#define SLB_ID_ASTAR_BACKTRACK_LOCK slb_tcb_nothing
#endif

template <ALG_TPARAMS_NO_DEFAULTS, template <class, bool> class BacktrackLock>
struct IdAstar; // Forward declare for the following type traits to work.

template <ALG_TPARAMS_NO_DEFAULTS, template <class, bool> class BacktrackLock>
struct AlgorithmTraits<IdAstar<ALG_TARGS, BacktrackLock>> {
    BASE_TRAITS_TYPES
    using MyAlgorithm = IdAstar<ALG_TARGS, BacktrackLock>;
};

/// The \c A* search algorithm.
/// \tparam logFlag If \c true, the events generated by the search algorithm are
/// logged. Otherwise, they are not.
/// \tparam Node_ The search node type.
/// \tparam GoalHandler The policy for handling goal conditions.
/// \tparam Heuristic The heuristic used by the search algorithm.
/// \tparam Open The open list type.
/// \note See the documentation of \ref ALG_TYPES and \ref ALG_DATA.
template <ALG_TPARAMS, template <class, bool> class BacktrackLock =
                           SLB_ID_ASTAR_BACKTRACK_LOCK>
struct IdAstar : Algorithm<IdAstar<ALG_TARGS, BacktrackLock>, ALG_TARGS> {
    BASE_TRAITS_TYPES

    using DirectBase = Algorithm<IdAstar<ALG_TARGS, BacktrackLock>, ALG_TARGS>;
    using GoalHandler = typename DirectBase::GoalHandler;
    using InitialHeuristic = typename DirectBase::InitialHeuristic;
    using Generator = typename DirectBase::Generator;
    using Neighbor = typename Generator::Neighbor;

    using MyType = IdAstar; ///< Required for \ref ALG_DATA symbol.
    using Backtrack = typename ::Backtrack<MyType, BacktrackLock>;
    ALG_DATA

    /// Initializes the algorithm based on the problem instance.
    /// \param instance The problem instance.
    IdAstar(const MyInstance &instance)
        : Base(instance), cur_{new Node(start_)}, backtrack_(*this) {}

    /// Runs the algorithm.
    /// \return The solution cost. If there is no solution, then \c CostType{-1}
    /// is returned.
    ReturnType run() {
        TimerLock lock{time_}; (void)lock;
        threshold_ = initialHeuristic_(cur_.get());
        cur_->set(0, threshold_, this->stamp());
        log<Events::MarkedStart>(log_, cur_.get());
        goalHandler_.logInit();

        while (true) {
            next_threshold_ = std::numeric_limits<CostType>::max();
            if (iteration()) break;
            threshold_ = next_threshold_;
        }

        cost_.set(res_);
        return res_;
    }

    /// Performs a single iteration of IDA*.
    /// \return \c true if the solution has been found and \c false otherwise.
    bool iteration() {
        if (cur_->f > threshold_) {
            next_threshold_ = std::min(next_threshold_, cur_->f);
            return false;
        }
        log<Events::Selected>(log_, cur_);
        goalHandler_.onSelect();
        if (goalHandler_.done()) return true;

        ++expanded_;
        auto neighbors_ = generator_.successors(cur_->state());
        for (auto &n : neighbors_) {
            ++generated_;
            auto btLock = backtrack_.lock(n); (void)btLock;
            if (iteration()) return true;
        }
        log<Events::Closed>(log_, cur_);
        return false;
    }

    /// \name Services for policies.
    /// @{
    Node *cur() { return &*cur_; }
    /// @}
private:
    std::unique_ptr<Node> cur_; ///< The currently selected node.
    Backtrack backtrack_;
    CostType threshold_, next_threshold_;
};

#endif
