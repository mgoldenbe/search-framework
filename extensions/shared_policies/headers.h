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
    using Log = typename AlgorithmTraits<MyAlgorithm>::MyAlgorithmLog;         \
    using CostType = typename AlgorithmTraits<MyAlgorithm>::CostType;          \
    using Node = typename AlgorithmTraits<MyAlgorithm>::Node;                  \
    using State = typename AlgorithmTraits<MyAlgorithm>::State;

#include "goal_handlers.h"

#endif
