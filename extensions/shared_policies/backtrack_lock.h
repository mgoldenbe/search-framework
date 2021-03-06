#ifndef SLB_EXTENSIONS_SHARED_POLICIES_BACKTRACK_LOCK_H
#define SLB_EXTENSIONS_SHARED_POLICIES_BACKTRACK_LOCK_H

/// \file
/// \brief Policies for handling backtracking in IDA*.
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace policy {

/// \namespace slb::ext::policy::backtrackLock
/// Policies for handling backtracking in IDA*.
namespace backtrackLock {

/// Base for a class handling RAII for backtracking in IDA* when the state is
/// updated in place (i.e. \ref core::sb::ActionNeighbor is used).
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm> struct InplaceBase {
    POLICY_TYPES

    /// The constructor.
    /// \tparam Neighbor The neighbor type.
    /// \param alg The search algorithm.
    template <class Neighbor>
    InplaceBase(MyAlgorithm &alg, Neighbor &) : alg_(alg), cur_(alg.cur()) {}

    /// Returns constant reference to the stored reverse action.
    /// \return constant reference to the stored reverse action.
    const typename State::Action &reverseAction() const {
        return reverseAction_;
    }

private:
    MyAlgorithm &alg_; ///< Reference to the search algorithm.
    NodeData data_;    ///< The data stored with the parent node.

    /// The action that takes back to the parent.
    typename State::Action reverseAction_;

protected:
    /// The node being expanded by IDA*.
    Node *cur_;

    /// Does the necessary bookkeeping for searching the given neighbor.
    /// \tparam Neighbor The neighbor type.
    /// \param n The neighbor.
    /// \param h The computed heuristic for \c n.
    template <class Neighbor> void set(Neighbor &n, CostType h) {
        data_ = *cur_;
        reverseAction_ = cur_->state().reverseAction(n.action());
        cur_->state().apply(n.action());
        cur_->set(cur_->g + n.cost(), h, alg_.stamp());
    }

    /// Does the necessary bookkeeping before backtracking to the parent.
    void unset() {
        cur_->NodeData::operator=(data_);
        cur_->state().apply(reverseAction_);
    }
};

/// Handles RAII for backtracking in IDA* when the state is updated in place.
/// \tparam MyAlgorithm The search algorithm.
/// \note The second template parameter is \c true when the log of algorithmic
/// events is kept (which necessitates storing the parent state) and \c false
/// otherwise.
template <class MyAlgorithm, bool = false>
struct Inplace : InplaceBase<MyAlgorithm> {
    POLICY_TYPES

    using Base = InplaceBase<MyAlgorithm>; ///< The base class.

    /// Whether the parent state is to be stored.
    static const bool keepsParent = false;

    /// The constructor.
    /// \tparam Neighbor The neighbor type.
    /// \param alg The search algorithm.
    /// \param n The neighbor.
    /// \param h The computed heuristic for \c n.
    template <class Neighbor>
    Inplace(MyAlgorithm &alg, Neighbor &n, CostType h) : Base(alg, n) {
        Base::set(n, h);
    }

    /// The moving constructor.
    Inplace(Inplace &&) = default;

    /// The destructor.
    ~Inplace() { Base::unset(); }
};

/// Handles RAII for backtracking in IDA* when the state is updated in place and
/// log is kept, so that a copy of the parent state needs to be kept.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
struct Inplace<MyAlgorithm, true> : InplaceBase<MyAlgorithm> {
    POLICY_TYPES

    using Base = InplaceBase<MyAlgorithm>; ///< The base class.
    using Base::cur_;

    /// Whether the parent state is to be stored.
    static const bool keepsParent = true;

    /// The constructor.
    /// \tparam Neighbor The neighbor type.
    /// \param alg The search algorithm.
    /// \param n The neighbor.
    /// \param h The computed heuristic for \c n.
    template <class Neighbor>
    Inplace(MyAlgorithm &alg, Neighbor &n, CostType h) : Base(alg, n) {
        set(n, h);
    }

    /// The moving constructor.
    Inplace(Inplace &&) = default;

    /// The destructor.
    ~Inplace() { unset(); }

private:
    std::unique_ptr<Node> parent_ = nullptr; ///< The parent node.

    /// Does the necessary bookkeeping for searching the given neighbor.
    /// \tparam Neighbor The neighbor type.
    /// \param n The neighbor.
    /// \param h The computed heuristic for \c n.
    template <class Neighbor> void set(Neighbor &n, CostType h) {
        parent_.reset(new Node(*cur_));
        cur_->setParent(&*parent_);
        Base::set(n, h);
    }

    /// Does the necessary bookkeeping before backtracking to the parent.
    void unset() {
        cur_->setParent(parent_->parent());
        Base::unset();
    }
};

/// A class for handling RAII for backtracking in IDA* when the parent state is
/// kept, i.e. \ref core::sb::StateNeighbor (i.e. not \ref
/// core::sb::ActionNeighbor) is used.
/// The second template parameter is not used, just for uniformity.
template <class MyAlgorithm, bool> struct Copy {
    POLICY_TYPES

    /// Whether the parent state is to be stored.
    static const bool keepsParent = true;

    /// The constructor.
    /// \tparam Neighbor The neighbor type.
    /// \param alg The search algorithm.
    /// \param n The neighbor.
    /// \param h The computed heuristic for \c n.
    template <class Neighbor>
    Copy(MyAlgorithm &alg, Neighbor &n, CostType h)
        : alg_(alg), cur_(alg.cur()) {
        set(n, h);
    }

    /// The moving constructor.
    Copy(Copy &&) = default;

    /// The destructor.
    ~Copy() { unset(); }

private:
    MyAlgorithm &alg_; ///< Reference to the search algorithm.

    /// The node being expanded by IDA*.
    Node *cur_;
    std::unique_ptr<Node> parent_ = nullptr; ///< The parent node.

    /// Does the necessary bookkeeping for searching the given neighbor.
    /// \tparam Neighbor The neighbor type.
    /// \param n The neighbor.
    template <class Neighbor> void set(Neighbor &n, CostType) {
        parent_.reset(new Node(*cur_));
        cur_->setState(std::move(alg_.generator().state(n)));
        auto h = alg_.generator().heuristic(n, cur_);
        cur_->set(cur_->g + n.cost(), h, alg_.stamp());
        cur_->setParent(&*parent_);
    }

    /// Does the necessary bookkeeping before backtracking to the parent.
    void unset() { std::swap(*cur_, *parent_); }
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
