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

std::string trim(const std::string &str,
                 const std::string &whitespace = " \t") {
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos) return ""; // no content
    const auto strEnd = str.find_last_not_of(whitespace);
    return str.substr(strBegin, strEnd - strBegin + 1);
}

std::vector<std::string> split(const std::string &s,
                               const std::vector<char> &delims) {
    std::vector<std::string> res;
    auto stuff = [&delims](char c) { return !in(delims, c); };
    auto space = [&delims](char c) { return in(delims, c); };
    auto first = std::find_if(s.begin(), s.end(), stuff);
    while (first != s.end()) {
        auto last = std::find_if(first, s.end(), space);
        res.push_back(std::string(first, last));
        first = std::find_if(last + 1, s.end(), stuff);
    }
    return res;
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
