///@file
///@brief INTERFACES CHECKED.

#ifndef UTIL_STREAM_H
#define UTIL_STREAM_H

template <typename T>
std::string str(const T& t) {
    std::ostringstream ss; ss << t; return ss.str();
}

std::string sureRead(std::ifstream &fin) {
    std::string res;
    fin >> res;
    assert(fin);
    return res;
}

std::string sureRead(std::ifstream &fin, const std::string &str) {
    std::string res = sureRead(fin);
    assert(res == str); (void)str;
    return res;
}

template <typename T>
void dumpT(const T& t) {
    std::cerr << t << std::endl;
}

// Output for all classes that define the dump member function accepting
// std::basic_ostream<charT>
// Note that dump() member function can work for other things such as Table
// If we were to not use dump(), there would be collisions with the output
// operators of Table.
template <typename charT, typename T>
auto operator<<(std::basic_ostream<charT> &o, const T &t)
    -> decltype(t.dump(o)) {
    return t.dump(o);
}

#endif
