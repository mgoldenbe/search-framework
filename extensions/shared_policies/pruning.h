#ifndef SLB_EXTENSIONS_SHARED_POLICIES_PRUNING_H
#define SLB_EXTENSIONS_SHARED_POLICIES_PRUNING_H

/// \file
/// \brief Policies for handling pruning in linear-space algorithms.
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace policy {

/// \namespace slb::ext::policy::pruning
/// Policies for handling pruning in linear-space algorithms.
namespace pruning {

/// No pruning.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm> struct Nothing {
    /// The constructor.
    Nothing(MyAlgorithm &) {}

    /// The call operator.
    /// \tparam Neighbor The neighbor type.
    /// \return Always \c false.
    template <class Neighbor> bool operator()(const Neighbor &) const {
        return false;
    }
};

/// Pruning the action that leads to a parent node.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
struct ToParent {
    /// The constructor.
    /// \param alg The search algorithm.
    ToParent(MyAlgorithm &alg) : alg_(alg) {}

    /// The call operator. Returns \c true if the given neighbor can be pruned.
    /// \tparam Neighbor The neighbor type.
    /// \param n The given neighbor.
    /// \return \c true if the given neighbor can be pruned and \c false otherwise.
    template <class Neighbor> bool operator()(const Neighbor &n) const {
        const auto &myLastLock = alg_.lastLock();
        return myLastLock && myLastLock->reverseAction() == n.action();
    }

private:
    MyAlgorithm &alg_; ///< Reference to the search algorithm.
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
