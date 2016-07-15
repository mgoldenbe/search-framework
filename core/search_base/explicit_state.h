#ifndef EXPLICIT_STATE_H
#define EXPLICIT_STATE_H

/// \file
/// \brief The \ref ExplicitState class and the related heuristics.
/// \author Meir Goldenberg

namespace Domains {

/// A type for common features of domains.
struct Base {
    /// Sets the space for this state class by reading it from the given file.
    static void initSpace(const std::string &) {}

    /// Returns the textual label for the vertex representing the state.
    /// \return The textual label for the vertex representing the state.
    std::string visualLabel() const {return "";}
};

/// The base class for a state in an explicit domain. It can be viewed as a
/// wrapper for the location type defined by the explicit domain.
/// \tparam ExplicitSpace The type for storing the explicit space (e.g. a map).
template<class ExplicitSpace>
struct ExplicitState: Base {
    /// The type representing the cost of actions in the domain.
    using CostType = typename ExplicitSpace::CostType;

    /// The type of a state in an explicit domain.
    using MyType = ExplicitState;

    /// The type used internally by \c ExplicitSpace to represent a state.
    /// To distinguish such a state from the state type used by search
    /// algorithms, we call the former \c location.
    using Location = typename ExplicitSpace::Location;

    /// Initialize the location with the space's default.
    ExplicitState() : loc_(space()->defaultLocation()) {}

    /// Initialize to the given location.
    /// \param loc The given location.
    explicit ExplicitState(const Location &loc) : loc_(loc) {}

    /// Initialize based on the string describing the state.
    /// \param s The string describing the state.
    explicit ExplicitState(const std::string &s) : loc_(space()->location(s)) {}

    /// Default copy constructor.
    ExplicitState(const MyType &) = default;

    /// Default assignment operator
    ExplicitState &operator=(const MyType &) = default;

    /// Computes the neighbors of the state.
    /// \tparam Neighbor The type for representing a single neighbor state.
    /// \return Vector of neighbors of the state.
    template<class Neighbor>
    std::vector<Neighbor> successors() const {
        std::vector<Neighbor> res;
        for (auto &n: space_->template neighbors<Neighbor>(loc_))
            res.push_back(std::move(n));
        return res;
    }

    /// Computes the hash code of the state.
    /// \return The hash code of the state.
    std::size_t hash() const {
        boost::hash<Location> myHash;
        return myHash(loc_);
    }

    /// Returns the location.
    /// \return The location.
    const Location &raw() const { return loc_; }

    /// Dumps the node the state to the given stream.
    /// \tparam Stream The output stream type.
    /// \param o The output stream.
    template <typename Stream>
    Stream &dump(Stream &o) const {
        return o << space()->locationStr(loc_);
    }

    /// The equality operator.
    /// \param rhs The right-hand side of the operator.
    bool operator==(const MyType &rhs) const {
        return loc_ == rhs.loc_;
    }

    /// Sets the space (i.e. an instance of ExplicitSpace) for this state class
    /// by reading it from the given file.
    /// \param fileName The name of the file to read the space from.
    static void initSpace(const std::string &fileName) {
        space_ = std::unique_ptr<ExplicitSpace>(new ExplicitSpace(fileName));
    }

    /// Returns a random location.
    /// \return A random location.
    static Location random() { return space()->random(); }

    /// Returns the space (i.e. an instance of ExplicitSpace) of this state
    /// class.
    /// \return Const reference to unique pointer to the the space (i.e. an
    /// instance of ExplicitSpace) of this state class.
    static const std::unique_ptr<ExplicitSpace> &space() {
        if (space_ == nullptr)
            throw std::logic_error("Explicit space has not been initialized");
        return space_;
    }
private:
    /// The location, which is the raw state as used by \c ExplicitSpace.
    Location loc_;
    static std::unique_ptr<ExplicitSpace> space_; ///< The explicit space.
};

template<class ExplicitSpace>
std::unique_ptr<ExplicitSpace> ExplicitState<ExplicitSpace>::space_ = nullptr;

/// The Manhattan heuristic functor.
struct ManhattanHeuristic {
    /// The function-call operator. Computes the heuristic.
    /// \tparam ExplicitSpace The type for storing the explicit space (e.g. a
    /// map).
    /// \param s The state from which the heuristic is to be computed.
    /// \param goal The goal state.
    /// \return The heuristic function from \c s to \c goal.
    template <class ExplicitSpace>
    typename ExplicitSpace::CostType
    operator()(const ExplicitState<ExplicitSpace> &s,
               const ExplicitState<ExplicitSpace> &goal) const {
        return ExplicitState<ExplicitSpace>::space()->manhattan(s.raw(),
                                                                goal.raw());
    }
};

/// The octile heuristic functor.
struct OctileHeuristic {
    /// The function-call operator. Computes the heuristic.
    /// \tparam ExplicitSpace The type for storing the explicit space (e.g. a
    /// map).
    /// \param s The state from which the heuristic is to be computed.
    /// \param goal The goal state.
    /// \return The heuristic function from \c s to \c goal.
    template <class ExplicitSpace>
    typename ExplicitSpace::CostType
    operator()(const ExplicitState<ExplicitSpace> &s,
               const ExplicitState<ExplicitSpace> &goal) const {
        return ExplicitState<ExplicitSpace>::space()->octile(s.raw(),
                                                             goal.raw());
    }
};

}
#endif
