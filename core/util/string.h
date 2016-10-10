///@file
///@brief String-related utilities.
/// \author Meir Goldenberg

#ifndef UTIL_STRING_H
#define UTIL_STRING_H

/// Trim white-space characters from the end of string.
/// \param str The string to be trimmed.
/// \param whitespace The characters to be considered as white-space.
/// \return The string after trimming.
std::string trim(const std::string &str,
                 const std::string &whitespace = " \t") {
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos) return ""; // no content
    const auto strEnd = str.find_last_not_of(whitespace);
    return str.substr(strBegin, strEnd - strBegin + 1);
}

/// Split the string using the giving characters.
/// \param s The string to be trimmed.
/// \param delims The characters to be used for splitting.
/// \return The strings resulting from the split.
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

/// Converts double to string, while getting rid of exponent notation and
/// trailing zeros.
/// \param f The number to be converted.
/// \return The string representation of `c`
/// \note The original code is taken from
/// http://stackoverflow.com/a/14966508/2725810. I modified the code to not
/// print numbers like 3.9999999999999999 or 3.00000000004. For such numbers,
/// only \c n90 9's none of the 0's are kept, so we get 3.99 and 3,
/// respectively.
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

/// Looks for the first substring that begins with a non-space character
/// and ends either at the end of string or with the given number of spaces.
/// The original string is modified to contain the non-parsed part of the
/// string.
/// \param s The string.
/// \param throwOnEmpty If \c true and not non-space character is found, an
/// exception is thrown.
/// \param nSpaces Number of spaces between tokens.
/// \return The found token.
/// \note \c s is modified to contain the non-parsed part of the
/// string.
std::string stuff(std::string &s, bool throwOnEmpty = false, int nSpaces = 2) {
    std::string res;

    auto begin = s.find_first_not_of(" ");
    if (begin == std::string::npos) return res;

    std::string space = std::string((unsigned)nSpaces, ' ');
    auto end = std::min(s.size(), s.find(space, begin));
    if (end == std::string::npos) end = s.size();

    res = std::string{s.begin() + begin, s.begin() + end};
    s = std::string(s.begin() + end, s.end());
    if (throwOnEmpty && res == "")
        throw std::runtime_error("Instance file ill-formed");
    return res;
}


#endif
