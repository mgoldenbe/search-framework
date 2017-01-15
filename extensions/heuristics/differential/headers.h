#ifndef SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_HEADERS_H
#define SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_HEADERS_H

/// \file
/// \brief The heuristics (not heuristic policies, so they do not accept the
/// algorithm type as a template parameter).
/// \author Meir Goldenberg

namespace slb {
namespace ext {

namespace algorithm {
struct SimpleUniformCost;
}

namespace heuristic {

/// \namespace slb::ext::heuristic::differential
/// The differential heuristic
namespace differential {}

} // namespace
} // namespace
} // namespace

// All the members of an index are static. An index without an inverse provides
// only the static constexpr member \c kind of the IndexKind type, the \c to
// function to compute an index based on step and a \c size
// member showing the size of the range of possible indices. An index with
// inverse provides in addition the \c from function to compute state based on
// index.
enum class IndexKind{none, noInverse, withInverse};
#include "explicit_domain_indices.h"
#include "differential.h"

#endif
