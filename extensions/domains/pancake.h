#ifndef PANCAKE_H
#define PANCAKE_H

/// \file
/// \brief The \ref Pancake class and the related heuristics.
/// \author Meir Goldenberg

namespace CommandLine {

/// Additions to the command line related to the Pancake puzzle domain.
struct Pancake {
    /// Returns the number of pancakes.
    /// \return The number of pancakes.
    int nPancakes() { return nPancakes_.getValue(); }

private:
    /// Command line option for the number of pancakes.
    TCLAP::ValueArg<int> nPancakes_;

protected:
    /// Injects this addition to the command line object.
    /// \param cmd The command-line object.
    Pancake(TCLAP::CmdLine &cmd_)
        : nPancakes_("", "nPancakes", "Number of pancakes", false, -1, "int",
                     cmd_) {}
};

}

namespace Domains {

/// The pancake puzzle domain.
struct Pancake: Base {
    /// The type representing the cost of actions in the domain. Every domain
    /// must provide this name.
    using CostType = int;

    /// The type for representing a single neighbor state. Every domain
    /// must provide this name.
    using Neighbor = StateNeighbor<Pancake>;

    /// Initializes the state with ordered pancakes.
    Pancake() : pancakes_(CMD.nPancakes()) {
        int i = -1;
        for (auto &el: pancakes_) el = ++i;
    }

    /// Initializes the state from a string, e.g. "[1, 4, 2, 3, 0]".
    /// \param s The string.
    Pancake(const std::string &s) {
        for (auto el: split(s, {' ', ',', '[', ']'}))
            pancakes_.push_back(std::stoi(el));
    }

    /// The default copy constructor.
    Pancake (const Pancake &) = default;

    /// The default assignment operator.
    /// \return Reference to the assigned state.
    Pancake &operator=(const Pancake &) = default;

    /// Returns the raw representation of the state as a vector.
    /// \return The raw representation of the state as a vector.
    const std::vector<int> &getPancakes() const { return pancakes_; }

    /// Performs a move, where by pancakes up to the position are reversed.
    /// \param pos The position up to (and including) which the pancakes are to
    /// be reversed.
    /// \return Pointer to the state after the move.
    Pancake *move(int pos) {
        std::reverse(pancakes_.begin(), pancakes_.begin() + pos + 1);
        return this;
    }

    /// Computes the neighbors of the state.
    /// \return Vector of neighbors of the state.
    std::vector<Neighbor> successors() const {
        std::vector<Neighbor> res;
        for (auto i = 1U; i != pancakes_.size(); ++i) {
            Neighbor cur((new Pancake(*this)) -> move(i));
            res.push_back(std::move(cur));
        }
        return res;
    }

    /// Computes the gap heuristic from the state to the goal state with ordered
    /// pancakes.
    /// \return The gap heuristic from the state to the goal state with ordered
    /// pancakes.
    int gapHeuristic() const {
        int res = 0;
        for (unsigned i = 0U; i < pancakes_.size() - 1; i++)
            if (abs(pancakes_[i] - pancakes_[i + 1]) > 1) res++;
        if (pancakes_.back() != (int)pancakes_.size() - 1) res++;
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
    template <class Stream>
    Stream &dump(Stream &o) const {
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
    /// \return \c true if the two states compare equal and \c false otherwise.
    bool operator==(const Pancake &rhs) const {
        return pancakes_ == rhs.pancakes_;
    }

    /// Returns a random state.
    /// \return A random state.
    static Pancake random() {
        Pancake res{};
        res.shuffle();
        return res;
    }
private:
    std::vector<int> pancakes_; ///< The underlying state representation.
};

}

//------------------------- HEURISTICS ------------------//

namespace Domains {

/// Functor for computing the gap heuristic to the goal state with ordered
/// pancakes.
struct GapHeuristic {
    /// The call operator. Computes the gap heuristic from the given state to
    /// the goal state with ordered pancakes.
    /// \param s The state from which the heuristic value is needed.
    /// \return The gap heuristic from \c s to the goal state with ordered
    /// pancakes.
    int operator()(const Pancake &s) const {
        return s.gapHeuristic();
    }
};

/// Functor for computing the gap heuristic to the given goal state.
struct GapHeuristicToGoal {
    /// The call operator. Computes the gap heuristic from the given state to
    /// the given goal state.
    /// \param s The state from which the heuristic value is needed.
    /// \param goal The goal state.
    /// \return The gap heuristic from \c s to \c goal.
    int operator()(const Pancake &s, const Pancake &goal) const {
        return s.gapHeuristic(goal);
    }
};

}

#endif
