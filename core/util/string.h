///@file
///@brief INTERFACES CHECKED.

#ifndef UTIL_STRING_H
#define UTIL_STRING_H

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

#endif
