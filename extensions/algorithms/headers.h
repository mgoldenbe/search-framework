#ifndef SLB_EXTENSIONS_ALGORITHMS_HEADERS_H
#define SLB_EXTENSIONS_ALGORITHMS_HEADERS_H

/// \file
/// \brief Implementation of the search algorithms.
/// \author Meir Goldenberg

namespace slb {
namespace ext {

/// \namespace slb::ext::algorithm
/// The search algorithms.
namespace algorithm {
using core::ui::AlgorithmLog;
}

} // namespace
} // namespace


#include "astar.h"
#include "per_goal.h"
#include "id_astar.h"

#endif
