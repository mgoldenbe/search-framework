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

/// Generic equality operator for shared pointers to the same state type.
/// \tparam State The state type, represents the domain.
/// \tparam Dummy Using *SFINAE* and the fact that all states define \c Neighbor
/// type to avoid catching all shared pointers.
/// \param lhs The right-hand side of the operator.
/// \param rhs The right-hand side of the operator.
/// \return \c true if the two states compare equal and \c false otherwise.
template <typename State, typename Dummy = typename State::Neighbor>
bool operator==(const std::shared_ptr <const State> &lhs,
                const std::shared_ptr <const State> &rhs) {
    return *lhs == *rhs;
}

// namespace std {
// template <typename State, typename NodeData>
// struct hash<SearchNode<State, NodeData>> {
//     std::size_t operator()(SearchNode<State, NodeData> const &key) const {
//         return key.state()->hash();
//     }
// };
// }

#endif
