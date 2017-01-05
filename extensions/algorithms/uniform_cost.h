#ifndef SLB_EXTENSIONS_ALGORITHMS_UNIFORM_COST_H
#define SLB_EXTENSIONS_ALGORITHMS_UNIFORM_COST_H

/// \file
/// \brief The \ref slb::ext::algorithm::UniformCost class.
/// \author Meir Goldenberg

#include "astar.h"

namespace slb {
namespace ext {
namespace algorithm {

using UniformNode = slb::core::sb::SearchNode<>;

template <class MyAlgorithm>
using UniformOL = ext::policy::openList::BucketedStdMap_T<
    MyAlgorithm, UniformNode, ext::policy::openList::DefaultOLKeyType,
    ext::policy::openList::GreaterPriority_SmallG,
    ext::policy::openList::OLMap>;

template <class MyAlgorithm>
using UniformHeuristic = ext::policy::heuristic::Zero<MyAlgorithm>;

template <class MyAlgorithm>
using UniformGenerator =
    ext::policy::generator::StatesT<MyAlgorithm, UniformHeuristic>;

/// \note It is defined as a struct rather than as an alias to allow forward
/// declarations.
template <bool logFlag, template <class> class GoalHandler = SLB_GOAL_HANDLER>
struct UniformCost : Astar<logFlag, UniformNode, GoalHandler, UniformHeuristic,
                           UniformGenerator, UniformOL> {
    using MyBase = Astar<logFlag, UniformNode, GoalHandler, UniformHeuristic,
                         UniformGenerator, UniformOL>;
    using MyBase::Astar;
};

/// \note It is defined as a struct rather than as an alias to allow forward
/// declarations.
struct SimpleUniformCost
    : UniformCost<false, ext::policy::goalHandler::NoGoal> {
    using MyBase = UniformCost<false, ext::policy::goalHandler::NoGoal>;
    using MyBase::UniformCost;
};

} // namespace
} // namespace
} // namespace

#endif
