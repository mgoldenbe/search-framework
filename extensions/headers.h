#ifndef SLB_EXTENSIONS_HEADERS_H
#define SLB_EXTENSIONS_HEADERS_H

/// \file
/// \brief Search algorithms, algorithmic variants and facilities that directly
/// support them.
/// \author Meir Goldenberg

namespace slb {

/// \namespace slb::ext
/// The user extensions.
namespace ext {
using namespace slb::core::sb;
using core::ui::log;
} // namespace

} // namespace

#include "explicit_spaces/headers.h"
#include "domains/headers.h"
#include "nodes/headers.h"
#include "events/headers.h"
#include "shared_policies/headers.h"
#include "algorithms/headers.h"
#include "instance_measures/headers.h"

#endif
