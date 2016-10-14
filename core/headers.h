#ifndef CORE_HEADERS_H
#define CORE_HEADERS_H

/// \file
/// \brief The core functionality of the framework.
/// \author Meir Goldenberg

/// The main namespace of the framework.
namespace slb {

/// Empty structure. Too bad this is not defined by the standard
struct Nothing {};

/// The trait used for some SFINAE magic.
template <typename...> struct voider {
    using type = void; ///< The type.
};

/// Used for some SFINAE magic.
template <typename... Ts> using void_t = typename voider<Ts...>::type;

/// \namespace slb::core
/// The core facilities of the framework.
namespace core {

#include "util/headers.h"
#include "core/command_line.h"
#include "search_base/headers.h"
#include "user_interface/visualizer.h"

} // namespace
} // namespace

#endif
