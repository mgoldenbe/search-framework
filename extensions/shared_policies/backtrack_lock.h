#ifndef BACKTRACK_LOCK_H
#define BACKTRACK_LOCK_H

/// \file
/// \brief Policies for handling backtracking in IDA*.
/// \author Meir Goldenberg

/// Base for a class handling RAII for backtracking in IDA* when the state is
/// updated in place.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
struct InplaceLockBase {
    POLICY_TYPES

    /// The constructor.
    /// \param cur The node being expanded by IDA*.
    template <class Neighbor>
    InplaceLockBase(MyAlgorithm &alg, Node *cur, Neighbor &)
        : alg_(alg), cur_(cur) {}

private:
    MyAlgorithm &alg_; ///< Reference to the search algorithm.
    NodeData data_;
    typename State::Action reverseAction_;

protected:
    /// The node being expanded by IDA*.
    Node *cur_;

    template <class Neighbor>
    void set(Neighbor &n) {
        data_ = *cur_;
        reverseAction_ = State::reverseAction(n.action());
        auto h = alg_.generator().heuristic(n, cur_);
        cur_->state().apply(n.action());
        cur_->set(cur_->g + n.cost(), h, alg_.stamp());
    }

    void unset() {
        cur_->NodeData::operator=(data_);
        cur_->state().apply(reverseAction_);
    }
};

/// Handles RAII for backtracking in IDA* when the state is updated in place.
/// \tparam MyAlgorithm The search algorithm.
/// \note The second template parameter is \c true when the log of algorithmic
/// events is kept and \c false otherwise.
template <class MyAlgorithm, bool = false>
struct InplaceLock : InplaceLockBase<MyAlgorithm> {
    POLICY_TYPES
    using Base = InplaceLockBase<MyAlgorithm>;

    static const bool keepsParent = false;

    template <class Neighbor>
    InplaceLock(MyAlgorithm &alg, Node *cur, Neighbor &n)
        : Base(alg, cur, n) {
        Base::set(n);
    }
    InplaceLock(InplaceLock &&) = default;

    ~InplaceLock() {
        Base::unset();
    }
};

/// Handles RAII for backtracking in IDA* when the state is updated in place and
/// log is kept, so that a copy of the parent state needs to be kept.
/// \tparam MyAlgorithm The search algorithm.
template <class MyAlgorithm>
struct InplaceLock<MyAlgorithm, true>: InplaceLockBase<MyAlgorithm> {
    POLICY_TYPES

    using Base = InplaceLockBase<MyAlgorithm>;
    using Base::cur_;

    static const bool keepsParent = true;

    template <class Neighbor>
    InplaceLock(MyAlgorithm &alg, Node *cur, Neighbor &n)
        : Base(alg, cur, n) {
        set(n);
    }
    InplaceLock(InplaceLock &&) = default;

    ~InplaceLock() {
        Base::unset();
    }
private:
    std::unique_ptr<Node> parent_ = nullptr;

    template <class Neighbor>
    void set(Neighbor &n) {
        parent_.reset(new Node(*cur_));
        cur_->setParent(&*parent_);
        Base::set(n);
    }

    void unset() {
        cur_->setParent(parent_->parent());
        Base::unset();
    }
};

/// A class for handling RAII for backtracking in IDA* when the parent state is
/// kept.
/// The second template parameter is not used, just for uniformity.
template <class MyAlgorithm, bool>
struct CopyLock {
    POLICY_TYPES

    static const bool keepsParent = true;

    /// The constructor.
    /// \param cur The node being expanded by IDA*.
    template <class Neighbor>
    CopyLock(MyAlgorithm &alg, Node *cur, Neighbor &n)
        : alg_(alg), cur_(cur) {
        set(n);
    }
    CopyLock(CopyLock &&) = default;

    ~CopyLock() {
        unset();
    }

private:
    MyAlgorithm &alg_; ///< Reference to the search algorithm.
    /// The node being expanded by IDA*.
    Node *cur_;
    std::unique_ptr<Node> parent_ = nullptr;

    template <class Neighbor>
    void set(Neighbor &n) {
        parent_.reset(new Node(*cur_));
        cur_->setState(std::move(alg_.generator().state(n)));
        auto h = alg_.generator().heuristic(n, cur_);
        cur_->set(cur_->g + n.cost(), h, alg_.stamp());
        cur_->setParent(&*parent_);
    }

    void unset() {
        std::swap(*cur_, *parent_);
    }
};

#endif
