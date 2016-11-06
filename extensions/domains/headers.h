#ifndef DOMAINS_HEADERS_H
#define DOMAINS_HEADERS_H

/// \file
/// \brief Implementation of the domains.
/// \author Meir Goldenberg

namespace slb {
namespace ext {

/// \namespace slb::ext::domain
/// The search domains.
namespace domain {}

} // namespace
} // namespace

#ifdef SLB_DOMAINS_INC_WORST
#include "inc_worst.h"
#endif

#ifdef SLB_DOMAINS_GRID_MAP
#include "grid_map_state.h"
#endif

#ifdef SLB_DOMAINS_PANCAKE
#include "pancake.h"
#endif

#endif
