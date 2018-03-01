#ifndef SLB_EXTENSIONS_DOMAINS_PANCAKE_H
#define SLB_EXTENSIONS_DOMAINS_PANCAKE_H

/// \file
/// \brief The Pancake puzzle domain and the related heuristics.
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace domain {

/// \namespace slb::ext::domain::pancake
/// The Pancake puzzle.
namespace pancake {

/// Additions to the command line related to the Pancake puzzle domain.
struct CommandLine {
    /// Returns the number of pancakes.
    /// \return The number of pancakes.
    int nPancakes() { return nPancakes_.getValue(); }

private:
    /// Command line option for the number of pancakes.
    TCLAP::ValueArg<int> nPancakes_;

protected:
    /// Injects this addition to the command line object.
    /// \param cmd The command-line object.
    CommandLine(TCLAP::CmdLine &cmd)
        : nPancakes_("", "nPancakes", "Number of pancakes", false, -1, "int",
                     cmd) {}
};

/// The pancake puzzle domain.
struct Pancake : DomainBase {
    /// The type representing the cost of actions in the domain. Every
    /// domain
    /// must provide this name.
    using CostType = int;

    using SNeighbor = StateNeighbor<Pancake>;  ///< State neighbor type.
    using ANeighbor = ActionNeighbor<Pancake>; ///< Action neighbor type.

    /// The type for representing an action. An action is the position up to
    /// (and including) which the pancakes are to be reversed.
    using Action = int;

    /// Initializes the state with ordered pancakes.
    /// \note The function is a template to avoid instantiation when the domain
    /// is not used. Such an instantiation would result in trying to use
    /// non-existing command line arguments.
    template <CMD_TPARAM> Pancake() {
        size_ = CMD_T.nPancakes();
        pancakes_ = new int8_t[size_];
        for (int i = 0; i < size_; ++i) pancakes_[i] = i;
    }

    /// Initializes the state from a string, e.g. "[1, 4, 2, 3, 0]".
    /// \param s The string.
    Pancake(const std::string &s) {
        const auto &strs = core::util::split(s, {' ', ',', '[', ']'});
        size_ = strs.size();
        pancakes_ = new int8_t[size_];
        for (int i = 0; i < size_; ++i) pancakes_[i] = std::stoi(strs[i]);
    }

    /// The default copy constructor.
    Pancake(const Pancake &s) {
        size_ = s.size_;
        pancakes_ = new int8_t[size_];
        std::copy(s.pancakes_, s.pancakes_ + size_, pancakes_);
    }

    /// The default assignment operator.
    /// \return Reference to the assigned state.
    Pancake &operator=(const Pancake &rhs) {
        Pancake tmp(rhs);
        swap(tmp);
        return *this;
    }

    ~Pancake() {
        delete[] pancakes_;
    }

    void swap(Pancake &s) {
        std::swap(size_, s.size_);
        std::swap(pancakes_, s.pancakes_);
    }

    /// Returns the raw representation of the state as a vector.
    /// \return The raw representation of the state as a vector.
    const int8_t *getPancakes() const { return pancakes_; }

    /// Applies an action to the state.
    /// \param a The action to be applied, i.e. the position up to (and
    /// including) which the pancakes are to be reversed.
    /// \return The state after the action.
    Pancake &apply(Action a) {
        std::reverse(pancakes_, pancakes_ + a + 1);
        return *this;
    }

    /// Computes the actions available from the state.
    /// \return Vector of actions available from the state.
    std::vector<Action> actions() const {
        std::vector<Action> res;
        for (auto i = 1U; i != size_; ++i) res.push_back(i);
        return res;
    }

    /// Returns the reverse of the given action.
    /// \param a The action whose reverse is to be returned.
    /// \return The reverse of the given action.
    static Action reverseAction(Action a) { return a; }

    /// Computes the state neighbors of the state.
    /// \return Vector of state neighbors of the state.
    std::vector<SNeighbor> stateSuccessors() const {
        std::vector<SNeighbor> res;
        for (auto a : actions()) {
            auto n = Pancake{*this}.apply(a);
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
        if (a < size_ - 1) {
            if (gap(a, a + 1) && !gap(0, a + 1)) return -1;
            if (gap(0, a + 1) && !gap(a, a + 1)) return 1;
            return 0;
        }

        // All pancakes are being reversed
        if (largestInPlace()) return 1;
        if (pancakes_[0] == size_ - 1) return -1;
        return 0;
    }

    /// Computes the gap heuristic from the state to the goal state with
    /// ordered
    /// pancakes.
    /// \return The gap heuristic from the state to the goal state with
    /// ordered
    /// pancakes.
    int gapHeuristic() const {
        int res = 0;
        for (unsigned i = 0U; i < size_ - 1; i++)
            if (gap(i, i + 1)) res++;
        if (!largestInPlace()) res++;
        return res;
    }

    /// Computes the gap heuristic from the state to the given goal state.
    /// \param goal The goal state.
    /// \return The gap heuristic from the state to \c goal.
    int gapHeuristic(const Pancake &goal) const {
        Pancake temp = *this;
        std::vector<int8_t> transform(goal.size_);
        for (auto i = 0U; i < goal.size_; ++i)
            transform[goal.pancakes_[i]] = i;
        for (auto i = 0U; i < temp.size_; ++i)
            temp.pancakes_[i] = transform[temp.pancakes_[i]];
        return temp.gapHeuristic();
    }

    /// Computes the hash-code of the state.
    /// \return The hash-code of the state.
    std::size_t hash() const {
        std::size_t seed = 0;
        for (auto i = 0U; i < size_; ++i)
            boost::hash_combine(seed, pancakes_[i]);
        return seed;
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
        Pancake old = *this;
        while (old == *this)
            std::random_shuffle(pancakes_, pancakes_ + size_);
    }

    /// The equality operator.
    /// \param rhs The right-hand side of the operator.
    /// \return \c true if the two states compare equal and \c false
    /// otherwise.
    bool operator==(const Pancake &rhs) const {
        return std::equal(pancakes_, pancakes_ + size_, rhs.pancakes_);
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
    /// The underlying state representation.
    /// Not using vector, since that would take 24 extra bytes per state.
    int8_t *pancakes_;
    int8_t size_;

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
        return (pancakes_[size_ - 1] == size_ - 1);
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
