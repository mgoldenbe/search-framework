#ifndef SLB_EXTENSIONS_DOMAINS_SLIDING_TILE_H
#define SLB_EXTENSIONS_DOMAINS_SLIDING_TILE_H

/// \file
/// \brief The sliding-tile puzzle domain and the related heuristics.
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace domain {

/// \namespace slb::ext::domain::sliding_tile
/// The sliding-tile puzzle.
namespace sliding_tile {

/// The sliding-tile puzzle domain.
/// \tparam nRows Number of rows on the board.
/// \tparam nRows Number of columns on the board.
template <int nRows, int nColumns>
struct SlidingTile : core::sb::DomainBase {
    /// The type representing the cost of actions in the domain. Every
    /// domain must provide this name.
    using CostType = int;

    using SNeighbor =
        core::sb::StateNeighbor<SlidingTile>; ///< State neighbor type.
    using ANeighbor =
        core::sb::ActionNeighbor<SlidingTile>; ///< Action neighbor type.

    /// The type for representing an action. The position of the tile being moved.
    using Action = int;

    /// Number of positions.
    static constexpr int size_ = nRows * nColumns;

    /// The type for the vector of actions for a given position of the blank.
    using BlankActions = std::vector<ANeighbor>;

    /// The type for all the actions in the domain.
    using AllActions = std::array<BlankActions, size_>;

    /// The type for two-dimension array of Manhattan distance heuristic deltas
    /// for a given tile. The indexes are from and to of an action.
    using TileMDDeltas = std::array<std::array<int, size_>, size_>;

    /// The type for all Manhattan distance heuristic deltas.
    using AllMDDeltas = std::array<TileMDDeltas, size_>;

    /// The type for raw state representation.
    using Board = std::array<int, size_>;

    /// Initializes the ordered state.
    SlidingTile() {
        int i = -1;
        for (auto &el : tiles_) el = ++i;
    }

    /// Initializes the state from a string, e.g. "[1, 4, 2, 3, 0, 5]" or "1 4 2
    /// 4 0 5" for 3x2 board.
    /// \param s The string.
    SlidingTile(const std::string &s) {
        int i = -1;
        for (auto el : core::util::split(s, {' ', ',', '[', ']'})) {
            tiles_[++i] = std::stoi(el);
            if (tiles_[i] == 0) blank_ = i;
        }
    }

    /// The default copy constructor.
    SlidingTile(const SlidingTile &) = default;

    /// The default assignment operator.
    /// \return Reference to the assigned state.
    SlidingTile &operator=(const SlidingTile &) = default;

    /// Returns the array of tiles at each position.
    /// \return The raw representation of the state, which is the array of tiles
    /// at each position..
    const Board &getTiles() const { return tiles_; }

    /// Applies an action to the state.
    /// \param a The action to be applied, i.e. the next position of the blank
    /// on the board.
    /// \return The state after the action.
    SlidingTile &apply(Action a) {
        tiles_[blank_] = tiles_[a];
        blank_ = a;
        return *this;
    }

    /// Returns the reverse of the given action in this state.
    /// \param a The action whose reverse is to be returned.
    /// \return The reverse of the given action.
    Action reverseAction(Action a) const {
        (void)a;
        return blank_;
    }

    /// Computes the state neighbors of the state.
    /// \return Vector of state neighbors of the state.
    std::vector<SNeighbor> stateSuccessors() const {
        std::vector<SNeighbor> res;
        for (auto a : actionSuccessors()) {
            auto n = SlidingTile{*this}.apply(a.action());
            res.push_back(std::move(n));
        }
        return res;
    }

    /// Computes the action neighbors of the state.
    /// \return Vector of action neighbors of the state.
    const std::vector<ANeighbor> &actionSuccessors() const {
        return allActions_()[blank_];
    }

    /// The change in the Manhattan distance heuristic to the goal state with
    /// ordered tiles and the blank at position 0 due to applying the given action.
    /// \param a The given action.
    /// \return The change in the Manhattan distance heuristic to the goal state
    /// with ordered pancake due to applying the given action.
    int mdDelta(Action a) const {
        return mdDeltas_()[tiles_[a]][a][blank_];
    }

    /// Computes the Manhattan distance heuristic to the goal state with
    /// ordered tiles and the blank at position 0.
    /// \return The Manhattan distance heuristic to the goal state with
    /// ordered tiles and the blank at position 0.
    int mdHeuristic() const {
        int res = 0;
        for (int pos = 0; pos < size_; ++pos)
            if (pos != blank_)
                res += rowDist(pos, tiles_[pos]) + colDist(pos, tiles_[pos]);
        return res;
    }

    /// Computes the hash-code of the state.
    /// \return The hash-code of the state.
    std::size_t hash() const {
        boost::hash<Board> v_hash;
        return v_hash(tiles_);
    }

    /// Dumps the state to the given stream.
    /// \tparam The stream type.
    /// \param o The stream.
    /// \return The modified stream.
    template <class Stream> Stream &dump(Stream &o) const {
        return o << tiles_;
    }

    /// Randomly shuffles the tiles.
    void shuffle() {
        auto old = tiles_;
        while (old == tiles_)
            std::random_shuffle(tiles_.begin(), tiles_.end());
    }

    /// The equality operator.
    /// \param rhs The right-hand side of the operator.
    /// \return \c true if the two states compare equal and \c false
    /// otherwise.
    bool operator==(const SlidingTile &rhs) const {
        if (blank_ != rhs.blank_) return false;
        for (int i = 0; i < size_; ++i)
            if (i != blank_ && tiles_[i] != rhs.tiles_[i]) return false;
        return true;
    }

    /// Returns a random state.
    /// \return A random state.
    static SlidingTile random() {
        SlidingTile res{};
        res.shuffle();
        return res;
    }

private:
    /// Tile at each position. This does not include the position of the blank,
    /// which is stored separately.
    std::array<int, size_> tiles_;

    /// Blank position.
    int blank_{};

    /// Computes the row number corresponding to the given position.
    /// \return The row number corresponding to the given position.
    static int row(int pos) { return pos / nColumns; }

    /// The difference between the row numbers corresponding to the two given
    /// positions.
    /// \return The difference between the row numbers corresponding to the two
    /// given positions.
    static int rowDiff(int pos1, int pos2) { return row(pos1) - row(pos2); }

    /// The distance between the row numbers corresponding to the two given
    /// positions.
    /// \return The distance between the row numbers corresponding to the two
    /// given positions.
    static int rowDist(int pos1, int pos2) {
        return std::abs(rowDiff(pos1, pos2));
    }

    /// Computes the column number corresponding to the given position.
    /// \return The column number corresponding to the given position.
    static int col(int pos) { return pos % nColumns; }

    /// The difference between the column numbers corresponding to the two given
    /// positions.
    /// \return The difference between the column numbers corresponding to the
    /// two given positions.
    static int colDiff(int pos1, int pos2) { return col(pos1) - col(pos2); }

    /// The distance between the column numbers corresponding to the two given
    /// positions.
    /// \return The distance between the column numbers corresponding to the
    /// two given positions.
    static int colDist(int pos1, int pos2) {
        return std::abs(colDiff(pos1, pos2));
    }

    /// Computes the actions available for each position of the blank.
    static AllActions computeAllActions() {
        AllActions res;
        for (int blank = 0; blank < size_; ++blank) {
            // the order is compatible with the code of Richard Korf.
            if (blank > nColumns - 1)
                res[blank].push_back(Action{blank - nColumns});
            if (blank % nColumns > 0)
                res[blank].push_back(Action{blank - 1});
            if (blank % nColumns < nColumns - 1)
                res[blank].push_back(Action{blank + 1});
            if (blank < size_ - nColumns)
                res[blank].push_back(Action{blank + nColumns});
        }
        return res;
    }

    /// Computes the heuristic updates for all the possible moves.
    /// \return The heuristic updates for all the possible moves.
    static AllMDDeltas computeAllMDDeltas() {
        AllMDDeltas res;
        for (int tile = 1; tile < size_; ++tile) {
            for (int blank = 0; blank < size_; ++blank) {
                for (const ANeighbor &a: allActions_()[blank]) {
                    int from = a.action(), to = blank;
                    res[tile][from][to] =
                        (rowDist(tile, to) - rowDist(tile, from)) +
                        (colDist(tile, to) - colDist(tile, from));
                }
            }
        }
        return res;
    }

    /// Returns all the actions.
    /// \note See http://stackoverflow.com/a/42208278/2725810
    static const AllActions& allActions_() {
        static const AllActions instance = computeAllActions();
        return instance;
    }

    /// Returns all the updates of the MD heuristic.
    static const AllMDDeltas& mdDeltas_() {
        static const AllMDDeltas instance = computeAllMDDeltas();
        return instance;
    }
};

//------------------------- HEURISTICS ------------------//

/// Functor for computing the Manhattan distance heuristic to the goal state
/// with ordered tiles and blank at position 0.
struct MDHeuristic {
    /// The constructor
    template <int nRows, int nColumns>
    MDHeuristic(const SlidingTile<nRows, nColumns> &) {}

    /// The call operator. Computes the MD heuristic from the given state to
    /// the goal state with ordered tiles and blank at position 0.
    /// \param s The state from which the heuristic value is needed.
    /// \return The MD heuristic from \c s to the goal state with ordered
    /// pancakes and blank at position 0.
    template <int nRows, int nColumns>
    int operator()(const SlidingTile<nRows, nColumns> &s) const {
        return s.mdHeuristic();
    }
};

/// Functor for computing the dynamic Manhattan distance heuristic to the goal state with ordered pancakes and blank at position 0.
struct DynamicMDHeuristic {
    /// The constructor
    template <int nRows, int nColumns>
    DynamicMDHeuristic(const SlidingTile<nRows, nColumns> &) {}

    /// The call operator. Dynamically computes the MD heuristic that results
    /// from applying a given action.
    /// \param parent The parent state.
    /// \param n The neighbor.
    template <class Neighbor, int nRows, int nColumns>
    int operator()(const SlidingTile<nRows, nColumns> &parent,
                   typename SlidingTile<nRows, nColumns>::CostType,
                   const Neighbor &n) const {
        return parent.mdDelta(n.action());
    }
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
