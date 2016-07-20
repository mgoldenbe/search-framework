#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
//#include <pair>
#include <algorithm>
#include <sstream>
#include <iomanip>

std::string double2str(double f) {
    const int n90 = 2;
    std::stringstream ss;

    ss << std::fixed << std::setprecision(40) << f;

    std::string s = ss.str();
    std::string::size_type len = s.length();

    // Removing zeros
    int zeros = 0;
    while (len > 1 && s[--len] == '0') zeros++;
    if (s[len] == '.') // remove final '.' if number ends with '.'
        zeros++;
    s.resize(s.length() - zeros);

    // Removing 9's and 0's etc.
    auto point = s.find('.');
    std::string s9(n90, '9'), s0(n90, '0');
    auto pos9 = s.find(s9, point + 1);
    auto pos0 = s.find(s0, point + 1);
    if (pos0 != std::string::npos && pos0 == point + 1)
        --pos0; // remove final '.'
    if (pos0 != std::string::npos || pos9 != std::string::npos)
        s.resize(std::min(pos0 - n90, pos9) + n90);

    return s;
}

int main() {
    std::string s1 = "aaa";
    std::string s2 = std::string(s1.begin() + 1, s1.begin() + 1);
    std::cout << s2.size() << std::endl;
    return 0;
}
