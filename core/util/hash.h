///@file
///@brief INTERFACES CHECKED.

#ifndef UTIL_HASH_H
#define UTIL_HASH_H

template <class State>
struct StateHash {
    std::size_t operator()(const State &s) const {
        return s.hash();
    }
};

template <class State>
struct StateSharedPtrHash {
    std::size_t operator()(const std::shared_ptr<const State> &s) const {
        return s->hash();
    }
};

/// Generic equality functor for shared pointers to the same state type to be
/// used for std::unordered_map.
/// \tparam State The state type, represents the domain.
template <class State>
struct StateSharedPtrEq {
    /// The equality operator for shared pointers to the same state type.
    /// \param lhs The right-hand side of the operator.
    /// \param rhs The right-hand side of the operator.
    /// \return \c true if the two states compare equal and \c false otherwise.
    bool operator()(const std::shared_ptr<const State> &lhs,
                  const std::shared_ptr<const State> &rhs) const {
        return *lhs == *rhs;
    }
};

/// The equality operator for shared pointers to the same state type.
/// \tparam State The state type, represents the domain.
/// \param lhs The right-hand side of the operator.
/// \param rhs The right-hand side of the operator.
/// \return \c true if the two states compare equal and \c false otherwise.
template <class State>
bool operator==(const std::shared_ptr<const State> &lhs,
                const std::shared_ptr<const State> &rhs) {
    return *lhs == *rhs;
}
#endif
