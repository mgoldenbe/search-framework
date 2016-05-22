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

// namespace std {
// template <typename State, typename NodeData>
// struct hash<SearchNode<State, NodeData>> {
//     std::size_t operator()(SearchNode<State, NodeData> const &key) const {
//         return key.state()->hash();
//     }
// };
// }

#endif
