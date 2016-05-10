#ifndef PER_GOAL_H
#define PER_GOAL_H

#include "algorithm.h"

// Per-Goal is a high-level algorithm that runs some other algorithm for each
// goal and combined results
template <ALG_TPARAMS, class MyAlgorithm = SLB_RAW_ALGORITHM<logFlag> >
struct PerGoal : Algorithm<ALG_TARGS> {
    ALG_TYPES
    ALG_DATA
    using Base::Algorithm;

    CostType run() {
        CostType res = 0.0;
        for (auto i : instance_.goalInstances()) {
            MyAlgorithm alg(i);
            res += alg.run();
            stats_.append(alg.measures());
        }
        return static_cast<double>(res) / instance_.goalInstances().size();
    }

    MeasureSet measures() const {
        return stats_.sum(true)[0];
    }

private:
    Stats stats_;
};

#endif
