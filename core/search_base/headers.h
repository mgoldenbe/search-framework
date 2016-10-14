#ifndef SEARCH_BASE_HEADERS_H
#define SEARCH_BASE_HEADERS_H

/// \file
/// \brief Facilities that support implementation of various heuristic search
/// algorithms.
/// \author Meir Goldenberg

/// \namespace slb::core::sb
/// The search base. These are facilities that support implementation of various
/// heuristic search algorithms.
namespace sb {

using util::operator<<; // Printing states.

#include "backtrack.h"
#include "instance.h"
#include "open_closed_list.h"
#include "domain_base.h"
#include "explicit_state.h"
#include "grid_map.h"
#include "neighbor.h"
#include "node.h"

} // namespace

#endif
