#ifndef SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_DIFFERENTIAL_H
#define SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_DIFFERENTIAL_H

/// \file
/// \brief The differential heuristic
/// \author Meir Goldenberg

#include "distance_map.h"
#include "distances_to_pivots.h"

namespace slb {
namespace ext {
namespace heuristic {

/// \namespace slb::ext::heuristic::differential
/// The differential heuristic
namespace differential {

/// Additions to the command line related to the differential heuristic.
struct CommandLine {
    /// Returns the number of pivots.
    /// \return The number of pivots.
    int nPivots() { return nPivots_.getValue(); }

private:
    /// Command line option for the number of pancakes.
    TCLAP::ValueArg<int> nPivots_;

protected:
    /// Injects this addition to the command line object.
    /// \param cmd The command-line object.
    CommandLine(TCLAP::CmdLine &cmd)
        : nPivots_("", "nPivots", "Number of pivots", false, -1, "int",
                     cmd) {}
};

struct Furthest {};

/// The differential heuristic.
/// \tparam State The state type representing the search domain.
template <class State, class BaseHeuristic, class Index = NoIndex>
struct HBase {
    using CostType = typename State::CostType;
    using Distances = DistancesToPivots<State, Index>;

    template <CMD_TPARAM>
    HBase(const State &goal)
        : nPivots_{CMD_T.nPivots()}, goal_{goal}, heuristic_(goal_) {
        distances_.updateExpectedNPivots(nPivots_);
    }

    CostType operator()(const State &s) const {
        CostType res = heuristic_(s);
        if (nPivots_ == 0) return res;
        auto it = distances_.pivotIt(s);
        for (int i = 0; i != nPivots_; ++i) {
            auto myH = it[i] - goalToPivots_[i];
            res = std::max(res, myH > CostType{0} ? myH : -myH);
        }
        return res;
    }

    protected:
        int nPivots_;
        static Distances distances_;
        const State &goal_;
        std::vector<CostType> goalToPivots_;
        BaseHeuristic heuristic_;

        virtual void myMakePivots() = 0;
        void makePivots() {
            if (nPivots_ != distances_.nPivots()) {
                distances_.clear();
                this->myMakePivots();
            }
        }
        void fillGoalToPivots() {
            if (nPivots_ == 0) return;
            auto it = distances_.pivotIt(goal_);
            for (int i = 0; i != nPivots_; ++i) goalToPivots_.push_back(it[i]);
        }
};
template <class State, class BaseHeuristic, class Index>
typename HBase<State, BaseHeuristic, Index>::Distances
    HBase<State, BaseHeuristic, Index>::distances_;

/// The differential heuristic. Specializations are for particular placements.
template <
    class State = SLB_STATE,
    class BaseHeuristic = SLB_DIFFERENTIAL_BASE_HEURISTIC,
    class Placement = SLB_DIFFERENTIAL_PLACEMENT,
    class IndexWithInverse = SLB_DIFFERENTIAL_INDEX_WITH_INVERSE, // = NoIndex
    class CompactIndex = SLB_DIFFERENTIAL_COMPACT_INDEX> // = IndexWithInverse
struct H;

/// The differential heuristic with the furthest placement.
/// \tparam State The state type representing the search domain.
template <class State, class BaseHeuristic, class IndexWithInverse,
          class CompactIndex>
struct H<State, BaseHeuristic, Furthest, IndexWithInverse, CompactIndex>
    : HBase<State, BaseHeuristic, CompactIndex> {
    static_assert(IndexWithInverse::kind != IndexKind::noInverse,
                  "Wrong type of index for IndexWithInverse");

    using MyBase = HBase<State, BaseHeuristic, CompactIndex>;
    using MyBase::nPivots_;
    using MyBase::distances_;

    H(const State &goal) : MyBase(goal) {
        this->makePivots();
        this->fillGoalToPivots();
    }

    virtual void myMakePivots() {
        // This has to have reverse to have farthest()
        DistanceMap<State, IndexWithInverse> toClosestPivot{firstPivot()};
        distances_.append(toClosestPivot);

        for (int i = 1; i != nPivots_; ++i) {
            DistanceMap<State, IndexWithInverse> pivotMap{toClosestPivot.farthest()};
            distances_.append(pivotMap);
            if (i != nPivots_ - 1) toClosestPivot.intersect(pivotMap);
        }
        //std::cerr << "Done making pivots!" << std::endl;
    }

private:
    State firstPivot() {
        DistanceMap<State, IndexWithInverse> temp{State{}};
        return temp.farthest();
    }
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
