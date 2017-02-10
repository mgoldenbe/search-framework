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
template <int nRows = SLB_SLIDING_TILE_NROWS,
          int nColumns = SLB_SLIDING_TILE_NCOLUMNS>
struct SlidingTile : DomainBase {
    /// The type representing the cost of actions in the domain. Every
    /// domain
    /// must provide this name.
    using CostType = int;

    using SNeighbor = StateNeighbor<SlidingTile>;  ///< State neighbor type.
    using ANeighbor = ActionNeighbor<SlidingTile>; ///< Action neighbor type.

    /// The type for representing an action. An action is represented by the
    /// direction of the blank's movement.
    enum class Action {up, down, left, right};

    /// The type for the vector of actions for a given position of the blank.
    using BlankActions = std::vector<Action>;

    /// The type for all the actions in the domain.
    using AllActions = std::array<BlankActions, size_>;

    static constexpr size_ = nRows * nColumns;
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
        for (auto el : core::util::split(s, {' ', ',', '[', ']'}))
            tiles_[++i] = std::stoi(el);
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
        tiles_[blank_] = tiles[a];
        blank_ = a;
        return *this;
    }

    /// Computes the actions available from the state.
    /// \return Vector of actions available from the state.
    const std::vector<Action> &actions() const {
        return allActions_[blank_];
    }

    /// Returns the reverse of the given action.
    /// \param a The action whose reverse is to be returned.
    /// \return The reverse of the given action.
    static Action reverseAction(Action a) {
        return reverseActions_[static_cast<int>(a)];
    }

    /// Computes the state neighbors of the state.
    /// \return Vector of state neighbors of the state.
    std::vector<SNeighbor> stateSuccessors() const {
        std::vector<SNeighbor> res;
        for (auto a : actions()) {
            auto n = SlidingTile{*this}.apply(a);
            res.push_back(std::move(n));
        }
        return res;
    }

    /// Computes the action neighbors of the state.
    /// \return Vector of action neighbors of the state.
    std::vector<ANeighbor> actionSuccessors() const {
        std::vector<ANeighbor> res;
        for (auto a : actions()) res.push_back(std::move(a));
        return res;
    }

    /// Computes the change in gap heuristic from the state obtained by
    /// applying
    /// the given action to the goal state with ordered pancakes.
    /// \return The gap heuristic from the state obtained by applying
    /// the given action to the goal state with ordered pancakes.
    int gapDelta(Action a) const {
        if (a < pancakes_.size() - 1) {
            if (gap(a, a + 1) && !gap(0, a + 1)) return -1;
            if (gap(0, a + 1) && !gap(a, a + 1)) return 1;
            return 0;
        }

        // All pancakes are being reversed
        if (largestInPlace()) return 1;
        if (pancakes_[0] == pancakes_.size() - 1) return -1;
        return 0;
    }

    // UP TO HERE

    /// Computes the gap heuristic from the state to the goal state with
    /// ordered
    /// pancakes.
    /// \return The gap heuristic from the state to the goal state with
    /// ordered
    /// pancakes.
    int gapHeuristic() const {
        int res = 0;
        for (unsigned i = 0U; i < pancakes_.size() - 1; i++)
            if (gap(i, i + 1)) res++;
        if (!largestInPlace()) res++;
        return res;
    }

    /// Computes the gap heuristic from the state to the given goal state.
    /// \param goal The goal state.
    /// \return The gap heuristic from the state to \c goal.
    int gapHeuristic(const Pancake &goal) const {
        Pancake temp = *this;
        std::vector<int> transform(goal.pancakes_.size());
        for (auto i = 0U; i < goal.pancakes_.size(); ++i)
            transform[goal.pancakes_[i]] = i;
        for (auto i = 0U; i < temp.pancakes_.size(); ++i)
            temp.pancakes_[i] = transform[temp.pancakes_[i]];
        return temp.gapHeuristic();
    }

    /// Computes the hash-code of the state.
    /// \return The hash-code of the state.
    std::size_t hash() const {
        boost::hash<std::vector<int>> v_hash;
        return v_hash(pancakes_);
    }

    /// Dumps the state to the given stream.
    /// \tparam The stream type.
    /// \param o The stream.
    /// \return The modified stream.
    template <class Stream> Stream &dump(Stream &o) const {
        return o << pancakes_;
    }

    /// Randomly shuffles the pancakes.
    void shuffle() {
        auto old = pancakes_;
        while (old == pancakes_)
            std::random_shuffle(pancakes_.begin(), pancakes_.end());
    }

    /// The equality operator.
    /// \param rhs The right-hand side of the operator.
    /// \return \c true if the two states compare equal and \c false
    /// otherwise.
    bool operator==(const Pancake &rhs) const {
        return pancakes_ == rhs.pancakes_;
    }

    /// Returns a random state.
    /// \return A random state.
    /// \note The function is a template to avoid instantiation when the domain
    /// is not used. Such an instantiation would result in trying to use
    /// non-existing command line arguments.
    template <CMD_TPARAM>
    static Pancake random() {
        Pancake res{};
        res.shuffle();
        return res;
    }

private:
    /// Tile at each position. This does not include the position of the blank,
    /// which is stored separately.
    std::array<int, size_> tiles_;

    /// Blank position.
    int blank_;

    const static AllActions allActions_ = computeAllActions();
    const static std::array<Action, 4> reverseActions_{
        Action::down, Action::up, Action::right, Action::left};

    /// Computes the position of the blank after the given action.
    /// \param a The given action.
    /// \return The position of the blank after \c a.
    int blankAfterAction(Action a) {
        switch (a) {
        case Action::up: return blank_ - nColumns;
        case Action::down: return blank_ + nColumns;
        case Action::left: return blank_ - 1;
        case Action::right: return blank_ - 1;
        default: assert(0);
        }
    }

    /// Computes the actions available for each position of the blank.
    static AllActions computeAllActions() {
        AllActions res;
        for (int blank = 0; blank < size_; ++blank) {
            if (blank > nColumns - 1) res[blank].push_back(Action::up);
            if (blank < size_ - nColumns) res[blank].push_back(Action::down);
            if (blank % nColumns > 0) res[blank].push_back(Action::left);
            if (blank % nColumns < nColumns - 1)
                res[blank].push_back(Action::right);
        }
        return res;
    }

    /// Is there a gap between positions i and i+1?
    /// \return \c true if there is a gap and \c false otherwise
    bool gap(int i) const { return gap(i, i + 1); }

    /// Is there a gap between positions i and j?
    /// \return \c true if there is a gap and \c false otherwise
    bool gap(int i, int j) const {
        return (abs(pancakes_[i] - pancakes_[j]) > 1);
    }

    /// Is the largest pancake in place?
    /// \return \c true if the largest pancake is in place and \c false
    /// otherwise
    bool largestInPlace() const {
        return (pancakes_.back() == (int)pancakes_.size() - 1);
    }
};

//------------------------- HEURISTICS ------------------//

/// Functor for computing the gap heuristic to the goal state with ordered
/// pancakes.
struct GapHeuristic {
    /// The constructor
    GapHeuristic(const Pancake &) {}

    /// The call operator. Computes the gap heuristic from the given state to
    /// the goal state with ordered pancakes.
    /// \param s The state from which the heuristic value is needed.
    /// \return The gap heuristic from \c s to the goal state with ordered
    /// pancakes.
    /// \note The last parameter is for reasons of uniformity, so the caller can
    /// pass the goal state.
    int operator()(const Pancake &s) const { return s.gapHeuristic(); }
};

/// Functor for computing the dynamic gap heuristic to the goal state with
/// ordered pancakes.
struct DynamicGapHeuristic {
    /// The constructor
    DynamicGapHeuristic(const Pancake &) {}

    /// The call operator. Dynamically computes the gap heuristic that results
    /// from applying a given action.
    /// \param parent The parent state.
    /// \param n The neighbor.
    template <class Neighbor>
    int operator()(const Pancake &parent, Pancake::CostType,
                   const Neighbor &n) const {
        return parent.gapDelta(n.action());
    }
};

/// Functor for computing the gap heuristic to the given goal state.
struct GapHeuristicToGoal {
    /// The constructor
    /// \param goal The goal state.
    GapHeuristicToGoal(const Pancake &goal) : goal_(goal) {}

    /// The call operator. Computes the gap heuristic from the given state to
    /// the given goal state.
    /// \param s The state from which the heuristic value is needed.
    /// \return The gap heuristic from \c s to \c goal_.
    int operator()(const Pancake &s) const { return s.gapHeuristic(goal_); }

private:
    const Pancake &goal_; ///< The goal state.
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
