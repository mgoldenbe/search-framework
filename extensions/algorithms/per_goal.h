#ifndef PER_GOAL_H
#define PER_GOAL_H

/// \file
/// \brief The \ref PerGoal class.
/// \author Meir Goldenberg

#include "algorithm.h"

/// \c PerGoal is a meta search algorithm for solving instances with multiple
/// goals. It runs some other search algorithm for each goal state and combines
/// results. Each goal state is solved independently of all others.
/// \tparam MyAlgorithm The search algorithm to run for each goal state.
template <ALG_TPARAMS, class MyAlgorithm = SLB_RAW_ALGORITHM<logFlag> >
struct PerGoal : Algorithm<ALG_TARGS> {
    ALG_TYPES
    ALG_DATA
    using Base::Algorithm; ///< Using the base's constructors.

    /// Runs the algorithm.
    /// \return The solution cost, which is the average of solution costs among
    /// the goals.
    /// \warning There is no special treatment for instances that do not have a
    /// solution at this point.
    CostType run() {
        CostType res = 0.0;
        for (auto i : instance_.goalInstances()) {
            MyAlgorithm alg(i);
            res += alg.run();
            stats_.append(alg.measures());
        }
        return static_cast<double>(res) / instance_.goalInstances().size();
    }

    /// Returns the statistics about the underlying search algorithm's
    /// performance for solving the particular instance for all goal states.
    /// \return The statistics about the underlying search algorithm's
    /// performance for solving the particular instance for all goal states.
    MeasureSet measures() const {
        return stats_.sum(true)[0];
    }

private:
    /// Statistics about the underlying search algorithm's performance for
    /// solving for all goal states.
    Stats stats_;
};

#endif
