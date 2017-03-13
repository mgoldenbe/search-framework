#ifndef SLB_EXTENSIONS_SHARED_POLICIES_GENERATORS_H
#define SLB_EXTENSIONS_SHARED_POLICIES_GENERATORS_H

/// \file
/// \brief Generators to be used by the search algorithms.
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace policy {

/// \namespace slb::ext::policy::generator
/// Generators to be used by the search algorithms.
namespace generator {

/// Generator that generates state neighbors.
/// \tparam MyAlgorithm The search algorithm.
/// \tparam Heuristic The heuristic policy.
template <class MyAlgorithm, template <class> class Heuristic_>
struct StatesT {
    POLICY_TYPES

    /// The search neighbor type.
    using Neighbor = typename State::SNeighbor;

    /// The heuristic type.
    using Heuristic = Heuristic_<MyAlgorithm>;

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

    /// Computes the heuristic.
    /// \param node The search node for which the heuristic is to be computed.
    /// \return The heuristic value.
    CostType heuristic(Node *node) const {
        return heuristic_(node);
    }

    /// Computes the successors of the given state.
    /// \param s The state.
    /// \return The vector of successors.
    /// \note This member could be static; made it non-static for reasons of
    /// uniformity.
    auto successors(const State &s) const -> decltype(s.stateSuccessors()) {
        return s.stateSuccessors();
    }
private:
    MyAlgorithm &alg_; ///< Reference to the search algorithm.
    Heuristic heuristic_; ///< The heuristic.
};
/// Generator that generates state neighbors.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
using States = StatesT<MyAlgorithm, SLB_HEURISTIC>;

/// Generator that generates action neighbors.
/// \tparam MyAlgorithm The search algorithm.
/// \tparam Heuristic The heuristic policy.
template <class MyAlgorithm, template <class> class Heuristic_>
struct ActionsT {
    POLICY_TYPES

    /// The search neighbor type.
    using Neighbor = typename State::ANeighbor;

    /// The heuristic type.
    using Heuristic = Heuristic_<MyAlgorithm>;

    /// The contructor.
    /// \param alg Reference to the search algorithm.
    ActionsT(MyAlgorithm &alg) : alg_(alg), heuristic_(alg) {}

    /// Returns the neighbor state.
    /// \return The neighbor state.
    State state(Neighbor &n) const {
        State res{alg_.cur()->state()};
        return res.apply(n.action());
    }

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
    auto successors(const State &s) const -> decltype(s.actionSuccessors()) {
        return s.actionSuccessors();
    }
private:
    MyAlgorithm &alg_; ///< Reference to the search algorithm.
    Heuristic heuristic_; ///< The heuristic.
};
/// Generator that generates action neighbors.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
using Actions = ActionsT<MyAlgorithm, SLB_HEURISTIC>;

} // namespace
} // namespace
} // namespace
} // namespace

#endif
