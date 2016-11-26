#ifndef SLB_DOMAIN_INC_WORST_H
#define SLB_DOMAIN_INC_WORST_H

/// \file
/// \brief The worst case example for an inconsistent heuristics (as appears on
/// page 75 of "Heuristic Search" by Stefan Edelkamp).
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace domain {

/// \namespace slb::ext::domain::incWorst
/// The worst case example for an inconsistent heuristics (as appears on
/// page 75 of "Heuristic Search" by Stefan Edelkamp).
namespace incWorst {

/// Additions to the command line related to the Pancake puzzle domain.
struct CommandLine {
    /// Returns the number of stages.
    /// \return The number of stages.
    int nStages() { return nStages_.getValue(); }

private:
    /// Command line option for the number of pancakes.
    TCLAP::ValueArg<int> nStages_;

protected:
    /// Injects this addition to the command line object.
    /// \param cmd The command-line object.
    CommandLine(TCLAP::CmdLine &cmd)
        : nStages_("", "nStages", "Number of stages for the IncWorst domain",
                   false, -1, "int", cmd) {}
};

/// The worst case example for an inconsistent heuristics.
struct IncWorst: DomainBase {
    /// The type representing the cost of actions in the domain. Every domain
    /// must provide this name.
    using CostType = int;

    /// The type for representing a single neighbor state. Every domain
    /// must provide this name.
    using SNeighbor = StateNeighbor<IncWorst, false>;

    using Row = enum {top, bottom}; ///< The type for row identification.

    /// Initializes the start state.
    /// \note The function is a template to avoid instantiation when the domain
    /// is not used. Such an instantiation would result in trying to use
    /// non-existing command line arguments.
    template <CMD_TPARAM>
    IncWorst() : k_(CMD_T.nStages() - 1), row_(bottom), n_(k_ - 1) {
        assert(legal(*this));
    }

    /// Initializes with the given number of stages, row and the stage number.
    /// \param k The number of stages.
    /// \param row The row.
    /// \param n The stage.
    IncWorst(int k, Row row, int n) : k_(k), row_(row), n_(n) {
        assert(legal(*this));
    }

    /// Initializes the state from a string, e.g. "[4, top, 2]".
    /// \note The function is a template to avoid instantiation when the domain
    /// is not used. Such an instantiation would result in trying to use
    /// non-existing command line arguments.
    /// \param s The string.
    template <CMD_TPARAM>
    IncWorst(const std::string &s) : k_(CMD_T.nStages() - 1) {
        auto v = core::util::split(s, {' ', ',', '[', ']'});
        row_ = (v[0] == "top" ? top : bottom);
        n_ = std::stoi(v[1]);
        if (!legal(*this)) throw std::invalid_argument("Bad state");
    }

    /// The default copy constructor.
    IncWorst (const IncWorst &) = default;

    /// The default assignment operator.
    /// \return Reference to the assigned state.
    IncWorst &operator=(const IncWorst &) = default;

    /// Computes the neighbors of the state.
    /// \return Vector of neighbors of the state.
    std::vector<SNeighbor> stateSuccessors() const {
        std::vector<SNeighbor> res;
        int nn = (n_ != 0 ? n_ - 1 : k_); // the neighbor stage
        switch (row_) {
        case bottom:
            res.push_back({IncWorst{k_, top, n_}, 1 << n_});
            if (n_ < k_) res.push_back({IncWorst{k_, bottom, nn}, 0});
            break;
        case top:
            if (n_ < k_)
                res.push_back({IncWorst{k_, bottom, nn}, -(1 << (n_ + 1))});
        }
        return res;
    }

    /// Computes the hash-code of the state.
    /// \return The hash-code of the state.
    std::size_t hash() const {
        boost::hash<std::vector<int>> v_hash;
        return v_hash({row_, n_});
    }

    /// Dumps the state to the given stream.
    /// \tparam The stream type.
    /// \param o The stream.
    /// \return The modified stream.
    template <class Stream>
    Stream &dump(Stream &o) const {
        return o << visualLabel();
    }

    /// Returns the textual label for the vertex representing the state.
    /// \return The textual label for the vertex representing the state.
    std::string visualLabel() const {
        int add = (n_ == k_? k_ : (k_ - 1) - n_);
        return str(static_cast<char>('A' + (1 - row_) * (k_ + 1) + add));
    }

    /// Returns the textual label for the edge to the given state.
    /// \param to The target state.
    /// \return The textual label for the edge to \c to.
    std::string visualLabel(const IncWorst &to) const {
        return str(cost(*this, to));
    }

    /// Fills out the coordinates for the vertex representing the state.
    /// \param x The x-coordinate to be filled out.
    /// \param y The y-coordinate to be filled out.
    void visualLocation(double &x, double &y) const {
        x = k_ - 1 - n_;
        if (n_ == k_) x = n_;
        x *= 2;
        if (row_ == top) ++x;
        y = row_;
    }

    /// The equality operator.
    /// \param rhs The right-hand side of the operator.
    /// \return \c true if the two states compare equal and \c false otherwise.
    bool operator==(const IncWorst &rhs) const {
        assert(k_ == rhs.k_);
        return (row_ == rhs.row_ && n_ == rhs.n_);
    }

    /// Returns a random state.
    /// \return A random state.
    /// \note The function is a template to avoid instantiation when the domain
    /// is not used. Such an instantiation would result in trying to use
    /// non-existing command line arguments.
    template <CMD_TPARAM>
    static IncWorst random() {
        return IncWorst{};
    }

private:
    /// The number of stages - 1.
    int k_;

    /// Top or bottom row.
    Row row_;

    /// The stage.
    /// Stages are numbered from right to left: k, 0, 1, ..., k-1.
    /// The goal is at stage k in the top row (in contrast to the book, where the
    /// goal is in the bottom row).
    /// The start is at stage k-1 in the bottom row.
    /// A node in the top row has the same number as the corresponding node
    /// diagonally to the left in the bottom row.
    int n_;

    /// Checks whether the state is legal.
    /// \param k The number of stages.
    /// \param row The row.
    /// \param n The stage.
    /// \return \c true if the state is legal and \c false otherwise.
    bool legal(int k, int row, int n) {
        if (k <= 1) return false;
        if (row != top && row != bottom) return false;
        if (n < 0 || n > k) return false;
        return true;
    }

    /// Checks whether the state is legal.
    /// \param s The state.
    /// \return \c true if the state is legal and \c false otherwise.
    bool legal(const IncWorst &s) {
        return legal(s.k_, s.row_, s.n_);
    }

    /// Returns the cost of the given move.
    /// \param from Source state.
    /// \param to Target state.
    /// \return The cost from \c from to \c to.
    int cost(const IncWorst &from, const IncWorst &to) const {
        for (auto &n: from.stateSuccessors())
            if (n.state() == to)
                return n.cost();
        assert(0);
    }
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
