///@file
///@brief INTERFACES CHECKED.

#ifndef UTIL_HEADERS_H
#define UTIL_HEADERS_H

/// \name Some useful simple declarations that do not deserve a
/// special file.
/// @{

struct Nothing {}; // too bad this is not defined by the standard

template <typename...>
struct voider { using type = void; };

template <typename... Ts>
using void_t = typename voider<Ts...>::type;
/// @}

#include "memory.h"
#include "hash.h"
#include "container.h"
#include "string.h"
#include "stream.h"
#include "table.h"
#include "time.h"
#include "vector_of_pairs.h"

#include "graphics/colors.h"
#include "graphics/curses_menu.h"
#include "graphics/geometry.h"
#include "graphics/drawing.h"
#include "graphics/x11.h"

#include "reflection/struct_diff.h"
#include "reflection/struct_2_stream.h"


#endif
