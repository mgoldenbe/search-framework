#ifndef SHARED_POLICIES_HEADERS_H
#define SHARED_POLICIES_HEADERS_H

/// \file
/// \brief Policy classes for search algorithms and algorithmic variants.
/// \author Meir Goldenberg

// See also http://stackoverflow.com/questions/8401827/crtp-and-type-visibility
template <typename MyAlgorithm> class AlgorithmTraits {};

/// Type aliases used by many policies.
#define POLICY_TYPES                                                           \
    using MyInstance = typename AlgorithmTraits<MyAlgorithm>::MyInstance;      \
    static constexpr bool logFlag = AlgorithmTraits<MyAlgorithm>::logFlag_;    \
    using Log = typename AlgorithmTraits<MyAlgorithm>::MyAlgorithmLog;         \
    using CostType = typename AlgorithmTraits<MyAlgorithm>::CostType;          \
    using Node = typename AlgorithmTraits<MyAlgorithm>::Node;                  \
    using NodeData = typename AlgorithmTraits<MyAlgorithm>::NodeData;          \
    using State = typename AlgorithmTraits<MyAlgorithm>::State;

#include "backtrack_lock.h"
#include "goal_handlers.h"
#include "heuristic_policies.h"
#include "generators.h"
#include "open_list.h"

#endif
