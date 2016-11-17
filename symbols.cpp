#include <iostream>
#include <fstream>
#include <regex>
#include <cassert>
#include <iomanip>

#include "core/util/container.h"
#include "core/util/stream.h"
#include "core/util/string.h"

using slb::core::util::str;
using slb::core::util::trim;

/// \file
/// \brief The tool for getting rid of undefined symbols used as default
/// template arguments.
///
/// The symbol may stand for a template, so that we might need to replace a
/// symbol that has its own template arguments. The tool exits with an error if
/// the brackets in the latter case are not matched (nested templates are
/// handled). It also exits with an error if an indication is found that an
/// undefined symbol is used in the context other than a default template
/// argument.
///
/// \author Meir Goldenberg

/// Like \c isalnum, but returns true for \c underscore
/// \param c The character
/// \return \c true if \c c is alphanumeric or underscore and \c false otherwise.
bool myalnum(char c) {
    return isalnum(c) || (c == '_');
}

/// Forms a message to the user from the given string.
/// \param s The string.
/// \return The message based on \c s.
std::string message(std::string s) {
    return "Symbols Tool: " + s;
}

/// Prints an error message based on the given string.
/// \param s The string.
void error(std::string s) {
    std::cerr << message("Error: " + s) << std::endl;
}

/// Prints a fatal error message based on the given string and exits.
/// \param s The string.
void fatal(std::string s) {
    std::cerr << message("Fatal Error: " + s + ". Exiting...") << std::endl;
    exit(-1);
}

/// Computes the sub-string of the given string. Handles reverse direction.
/// \param s The string.
/// \param b The beginning position of the range.
/// \param e The position one beyond the end of the range.
/// \return The sub-string \c s[b:e).
std::string mysubstr(std::string s, int b, int e) {
    if (e > b) return s.substr(b, e - b);
    return s.substr(e + 1, b - e);
}

/// Returns the closing bracket for the corresponding opening one.
/// \param openingBracket The opening bracket character.
/// \return The closing bracket character corresponding to \c openingBracket.
char closingBracket(char openingBracket) {
    switch(openingBracket) {
    case '<' : return '>';
    case '>' : return '<';
    case '(' : return ')';
    case ')' : return '(';
    case '[' : return ']';
    case ']' : return '[';
    default: assert(0);
    }
}

/// Finds the matching closing bracket.
/// \param str The source string.
/// \param it Iterator to the position of the opening bracket.
/// \param direction The direction of search. Must be either 1 or -1.
/// \return Iterator to the matching closing bracket.
std::string::const_iterator matchBracket(const std::string &str,
                                         std::string::const_iterator it,
                                         int direction = 1) {
    assert(direction == 1 || direction == -1);
    char opening = *it;
    char closing = closingBracket(opening);
    int openCount = 1;
    while (openCount) {
        int pos = it - str.cbegin();
        if (pos + direction < 0 || pos + direction > str.size() - 1)
            fatal("Could not match brackets");
        it += direction;
        if (*it == opening) ++openCount;
        if (*it == closing) --openCount;
    }
    return it;
}

/// Finds the matching closing bracket.
/// \param str The source string.
/// \param pos Position right after the opening bracket.
/// \param direction The direction of search. Must be either 1 or -1.
/// \return Position of the matching closing bracket.
int matchBracket(const std::string &str, int pos, int direction = 1) {
    return matchBracket(str, str.cbegin() + pos, direction) - str.cbegin();
}

/// The token type.
struct Token {
    std::string t; ///< The token.
    int b; ///< The position of the token in the original string.
    int e; ///< The position one beyond the end of token in the original string.
           ///< If looking in reverse direction, then e<b.
};

/// Finds the next token in the given direction. A matched bracket is considered
/// to be one token. Disregards whitespace characters.
/// \tparam direction The direction of the search. Must be 1 or -1.
/// \param str The string.
/// \param pos The position where the token begins.
/// \return The parsed token.
template <int direction>
Token token(std::string str, int pos) {
    assert(direction == 1 || direction == -1);
    Token res;
    constexpr char openBracket = (direction == 1 ? '<' : '>');
    constexpr char closeBracket = (direction == 1 ? '>' : '<');
    // skip whitespace
    while (isspace(str[pos])) pos += direction;
    res.b = pos;
    switch (str[pos]) {
    case ',':
    case closeBracket:
        res.e = pos + direction;
        break;
    case openBracket:
        res.e = matchBracket(str, pos, direction) + direction;
        break;
    default:
        if (!myalnum(str[pos]))
            fatal("Non-handled character '" + std::string{str[pos]} + "'");
        while (myalnum(str[pos])) pos += direction;
        res.e = pos;
    }
    res.t = mysubstr(str, res.b, res.e);
    return res;
}

/// Determines the type of the template argument by the position where the
/// default template argument begins.
/// \param s The source.
/// \param pos The position of '=', where the default template argument begins.
/// \return The type of the template argument.
std::string argType(const std::string &s, int pos) {
    std::vector<Token> tokens;
    while (true) {
        Token t = token<-1>(s, pos);
        if (t.t == "<" || t.t == ",") break;
        pos = t.e;
        tokens.push_back(t);
    }
    std::reverse(tokens.begin(), tokens.end());
    std::string res = "";
    for (const auto &t_: tokens) res += t_.t + " ";

    // Take names out
    // std::cout << message("Was: " + res) << std::endl;
    std::regex e1{R"(class\s*\w*\s*)"};
    res = std::regex_replace(res, e1, "class");
    std::regex e2{R"(typename\s*\w*\s*)"};
    res = std::regex_replace(res, e2, "class");
    std::regex e3{R"(struct\s*\w*\s*)"};
    res = std::regex_replace(res, e3, "class");
    // std::cout << message("Became: " + res) << std::endl;
    return res;
}

// http://stackoverflow.com/a/37259671/2725810
/// Computes the matches of a regex search.
/// \param s The string in which to search.
/// \param exp The regular expression.
/// \return The matches of the regex search.
std::vector<std::smatch> matches(std::string s, std::string exp) {
    std::vector<std::smatch> res;
    std::regex r{exp};
    for (auto i = std::sregex_iterator(s.begin(), s.end(), r);
         i != std::sregex_iterator(); ++i)
        res.push_back(*i);
    return res;
}

/// The main function.
int main(int argc, char **argv) {
    if (argc < 2)
        throw std::invalid_argument("Need to supply the input file");

    std::ifstream in(argv[1]);
    if (!in) fatal("Could not open file " + std::string(argv[1]));
    // The initial source code
    std::string source((std::istreambuf_iterator<char>(in)),
                       std::istreambuf_iterator<char>());

    // The target source code
    std::string target = "";

    // The types to be added to the target at the end
    std::string types = "";

    // The map of nothing types
    std::map<std::string, std::string> nothings;

    int segmentBegin = 0;
    for (const auto &m: matches(source, R"(=\s*SLB_\w*\s*<?)")) {
        // Keep the part of the string up to the beginning of the pattern.
        target += mysubstr(source, segmentBegin, m.position());

        // After pattern or closing bracket if there was one.
        int patternLast = m.position() + m.length() - 1;
        segmentBegin =
            (source[patternLast] == '<' ? matchBracket(source, patternLast) + 1
                                        : patternLast + 1);

        std::string forRemoval =
            trim(mysubstr(source, m.position() + 1, segmentBegin));

        // Check that we are in the right context: the next non-whitespace
        // character is either a comma or a closing bracket.
        auto nonspace = find_if(source.cbegin() + segmentBegin, source.cend(),
                                std::not1(std::ref(isspace)));
        if (*nonspace != ',' && *nonspace != '>')
            fatal("Undefined symbol " + forRemoval +
                    " not in the default template argument context.");

        std::string mytype = argType(source, m.position() - 1);
        std::string &mynothing = nothings[mytype];
        if (mynothing == "") {
            mynothing = "Nothing" + str(nothings.size());
            types += mytype + " " + mynothing + " {};\n";
        }
        target += "= " + mynothing;

        // std::cout << message("Substituting: ") << forRemoval << " for "
        //           << mynothing << std::endl;
    }
    target += mysubstr(source, segmentBegin, source.size());

    // Checking that the target string does not contain any undefined symbols.
    bool errorFlag = false;

    for (const auto &m: matches(target, R"(SLB_\w*)")) {
        errorFlag = true;
        error("Undefined symbol " +
              m.str() + " not in the default template argument context.");
    }
    if (errorFlag)
        fatal("There are undefined symbols not in the default template "
                "argument context.");
    std::ofstream out(std::string{argv[1]});
    out << types << target << std::endl;
    //std::ofstream out1(std::string{argv[1]} + ".orig");
    //out1 << source << std::endl;
    return 0;
}
