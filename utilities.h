///@file
///@brief INTERFACES CHECKED.

#ifndef UTILITIES
#define UTILITIES

#include <sstream>
#include <memory>

template <typename T>
std::string str(const T& t) {
    std::ostringstream ss; ss << t; return ss.str();
}

template <typename T>
void dumpT(const T& t) {
    std::cerr << t << std::endl;
}

// Output for all classes that define the dump member function accepting
// std::basic_ostream<charT>
template <typename charT, typename T>
auto operator<<(std::basic_ostream<charT> &o, const T &t)
    -> decltype(t.dump(o)) {
    return t.dump(o);
}

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
// struct hash<AStarNode<State, NodeData>> {
//     std::size_t operator()(AStarNode<State, NodeData> const &key) const {
//         return key.state()->hash();
//     }
// };
// }

#endif
