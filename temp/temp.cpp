#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cassert>
#include <functional>
#include <algorithm>
#include "PrettyPrint.h"

// Look for the first substring that begins with a non-space character
// and ends either at the end of string or with nSpaces spaces.
// The original string is changed for the non-parsed part of the string.
std::string stuff(std::string &s, int nSpaces) {
    std::string res;
    auto begin = s.find_first_not_of(" ");
    std::string space = std::string((unsigned)nSpaces, ' ');
    auto end = std::min(s.size(), s.find(space, begin));
    res = std::string{s.begin() + begin, s.begin() + end};
    s = std::string(s.begin() + end, s.end());
    return res;
}

int main()
{
    std::string s = "abcd dd   rg";
    std::cout << stuff(s, 2) << std::endl;
    std::cout << s << std::endl;
    std::cout << stuff(s, 2) << std::endl;
    std::cout << s.size() << std::endl;
    return 0;
}
