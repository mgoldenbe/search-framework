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

template <bool logFlag, template <class> class GoalHandler = SLB_GOAL_HANDLER>
using UniformCost =
    Astar<logFlag, UniformNode, GoalHandler, ext::policy::heuristic::Zero,
          ext::policy::generator::States, UniformOL>;

using SimpleUniformCost =
    Astar<false, UniformNode, ext::policy::goalHandler::NoGoal,
          ext::policy::heuristic::Zero, ext::policy::generator::States,
          UniformOL>;

} // namespace
} // namespace
} // namespace

#endif
