#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

template<class V, typename T>
bool in(const V &v, const T &el) {
    return std::find(v.begin(), v.end(), el) != v.end();
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

int main() {
    std::string s = " [1, 2, 3 ] ";
    for (auto el: split(s, {' ', ',', '[', ']'}))
        std::cout << el << std::endl;
    return 0;
}
