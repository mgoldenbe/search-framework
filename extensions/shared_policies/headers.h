#ifndef SHARED_POLICIES_HEADERS_H
#define SHARED_POLICIES_HEADERS_H

/// \file
/// \brief Policy classes for search algorithms and algorithmic variants.
/// \author Meir Goldenberg

// See also http://stackoverflow.com/questions/8401827/crtp-and-type-visibility

/// \namespace slb::ext::algorithm
namespace algorithm {
template <typename MyAlgorithm> struct AlgorithmTraits;
} // namespace

/// Type aliases used by many policies.
#define POLICY_TYPES                                                           \
    using Traits = ext::algorithm::AlgorithmTraits<MyAlgorithm>;               \
    using MyInstance = typename Traits::MyInstance;                            \
    static constexpr bool logFlag = Traits::logFlag_;                          \
    using Log = typename Traits::MyAlgorithmLog;                               \
    using CostType = typename Traits::CostType;                                \
    using Node = typename Traits::Node;                                        \
    using NodeData = typename Traits::NodeData;                                \
    using State = typename Traits::State;

/// \namespace slb::ext::policy
/// Policy classes for search algorithms and algorithmic variants.
namespace policy {

#include "backtrack_lock.h"
#include "goal_handlers.h"
#include "heuristic_policies.h"
#include "generators.h"
#include "open_list.h"

} // namespace

#endif
