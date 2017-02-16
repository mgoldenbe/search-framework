#ifndef SLB_EXTENSIONS_DOMAINS_SLIDING_TILE_FWD_H
#define SLB_EXTENSIONS_DOMAINS_SLIDING_TILE_FWD_H

/// \file
/// \brief Forward declarations for \ref sliding_tile.h.
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace domain {
namespace sliding_tile {


#ifndef SLB_SLIDING_TILE_NROWS
#define SLB_SLIDING_TILE_NROWS 4
#endif

#ifndef SLB_SLIDING_TILE_NCOLUMNS
#define SLB_SLIDING_TILE_NCOLUMNS 4
#endif

template <int nRows = SLB_SLIDING_TILE_NROWS,
          int nColumns = SLB_SLIDING_TILE_NCOLUMNS>
struct SlidingTile;

} // namespace
} // namespace
} // namespace
} // namespace

#endif
