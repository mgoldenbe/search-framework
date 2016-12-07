#ifndef SLB_SLB_H
#define SLB_SLB_H

/// \file
/// \brief This header file includes the whole framework.
/// \author Meir Goldenberg

/// \namespace slb
/// The main namespace of the framework.
namespace slb {}

// Can be pre-compiled (~25% compile-time reduction)
#include "outside_headers.h"

#include "extensions/headers_fwd.h"
#include "core/headers.h"
#include "extensions/headers.h"

#endif
