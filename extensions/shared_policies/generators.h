#ifndef GENERATORS_H
#define GENERATORS_H

/// \file
/// \brief Generators to be used by the search algorithms.
/// \author Meir Goldenberg

/// \namespace ext::policy::generator
/// Generators to be used by the search algorithms.
namespace generator {

/// Generator that generates state neighbors.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm, template <class> class Heuristic>
struct StatesT {
    POLICY_TYPES

    using Neighbor = typename State::SNeighbor;

    /// The contructor.
    /// \param alg Reference to the search algorithm.
    StatesT(MyAlgorithm &alg) : alg_(alg), heuristic_(alg) {}

    /// Returns the neighbor state.
    /// \return Reference to the neighbor state.
    /// \note This reference must be non-const to allow moving the neighbor. In
    /// addition, this member could be static; made it non-static for reasons of
    /// uniformity.
    State &state(Neighbor &n) { return n.state(); }

    /// Computes the heuristic.
    /// \param n The neighbor for which the heuristic is to be computed.
    /// \param node The search node for which the heuristic is to be computed.
    /// \return The heuristic value.
    CostType heuristic(const Neighbor &n, Node *node) const {
        return heuristic_(n, node);
    }

    /// Computes the successors of the given state.
    /// \param s The state.
    /// \return The vector of successors.
    /// \note This member could be static; made it non-static for reasons of
    /// uniformity.
    std::vector<Neighbor> successors(const State &s) const {
        return s.stateSuccessors();
    }
private:
    MyAlgorithm &alg_; ///< Reference to the search algorithm.
    Heuristic<MyAlgorithm> heuristic_; ///< The heuristic.
};
template <class MyAlgorithm>
using States = StatesT<MyAlgorithm, SLB_HEURISTIC>;

/// Generator that generates action neighbors.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm, template <class> class Heuristic>
struct ActionsT {
    POLICY_TYPES

    using Neighbor = typename State::ANeighbor;

    /// The contructor.
    /// \param alg Reference to the search algorithm.
    ActionsT(MyAlgorithm &alg) : alg_(alg), heuristic_(alg) {}

    /// Returns the neighbor state.
    /// \return Reference to the neighbor state.
    /// \note This reference must be non-const to allow moving the neighbor
    State state(Neighbor &n) const {
        State res{alg_.cur()->state()};
        return res.apply(n.action());
    }

    /// Computes the heuristic.
    /// \param n The neighbor for which the heuristic is to be computed.
    /// \return The heuristic value.
    CostType heuristic(const Neighbor &n, Node *node) const {
        return heuristic_(n, node);
    }

    /// Computes the successors of the given state.
    /// \param s The state.
    /// \return The vector of successors.
    /// \note This member could be static; made it non-static for reasons of
    /// uniformity.
    std::vector<Neighbor> successors(const State &s) const {
        return s.actionSuccessors();
    }
private:
    MyAlgorithm &alg_; ///< Reference to the search algorithm.
    Heuristic<MyAlgorithm> heuristic_; ///< The heuristic.
};
template <class MyAlgorithm>
using Actions = ActionsT<MyAlgorithm, SLB_HEURISTIC>;

} // namespace

#endif
