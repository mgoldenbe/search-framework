///@file
///@brief INTERFACES CHECKED.

#ifndef UTILITIES
#define UTILITIES

#include <sstream>
#include <memory>
#include <time.h>
#include <algorithm>

// http://cc.byexamples.com/2007/05/25/nanosleep-is-better-than-sleep-and-usleep/
void __nsleep(const struct timespec *req, struct timespec *rem) {
    struct timespec temp_rem;
    if (nanosleep(req, rem) == -1)
        __nsleep(rem, &temp_rem);
}

int msleep(unsigned long milisec) {
    struct timespec req = {0,0}, rem = {0,0};
    time_t sec = (int)(milisec / 1000);
    milisec = milisec - (sec * 1000);
    req.tv_sec = sec;
    req.tv_nsec = milisec * 1000000L;
    __nsleep(&req, &rem);
    return 1;
}

template<class V, typename T>
bool in(const V &v, const T &el) {
    return std::find(v.begin(), v.end(), el) != v.end();
}

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
