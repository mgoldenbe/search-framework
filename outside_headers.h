#ifndef OUTSIDE_H
#define OUTSIDE_H

/// \file
/// \brief Inclusion of standard and third-party libraries. This header is
/// well-suited for pre-compilation.
/// \author Meir Goldenberg

#include <cassert>
#include <cctype>
#include <chrono>
#include <climits>
#include <csignal>
#include <cmath>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <memory>
#include <type_traits>
#include <time.h>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <utility>                   // for std::pair

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <cairo-xlib.h>
#include <X11/XKBlib.h>
//#include <X11/extensions/XTest.h>

#include <boost/functional/hash.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/circle_layout.hpp>
#include <boost/graph/kamada_kawai_spring_layout.hpp>
#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/property_map/transform_value_property_map.hpp>


#include <tclap/CmdLine.h>
#include <reflect_struct.h>
#include <pretty_print.h>
#include <ncurses.h>
#include <menu.h>
#include <cairo.h>

#endif
