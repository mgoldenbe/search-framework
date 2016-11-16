#include <iostream>
#include <fstream>
#include <regex>

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

/// Finds the matching closing bracket.
/// \param str The source string.
/// \param it Iterator to the position right after the opening bracket.
/// \return Iterator to the matching closing bracket.
std::string::const_iterator matchBracket(const std::string &str,
                                         std::string::const_iterator it) {
    if (it == str.cbegin()) throw std::runtime_error("Matching error.");
    --it; // '<' was part of the pattern
    if (*it != '<') return it + 1;
    int openCount = 1;
    while (++it != str.cend() && openCount) {
        if (*it == '<') ++openCount;
        if (*it == '>') --openCount;
    }
    if (openCount) throw std::runtime_error("Could not match brackets.");
    return it;
}

/// Finds the matching closing bracket.
/// \param str The source string.
/// \param pos Position right after the opening bracket.
/// \return Position of the matching closing bracket.
int matchBracket(const std::string &str, int pos) {
    return matchBracket(str, str.cbegin() + pos) - str.cbegin();
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

/// Computes the sub-string of the given string.
/// \param s The string.
/// \param b The beginning position of the range.
/// \param s The position one beyond the end of the range.
std::string mysubstr(std::string s, int b, int e) { return s.substr(b, e - b); }

/// Forms a message to the user.
std::string message(std::string s) {
    return "Symbols Tool: " + s;
}

/// Prints an error message.
void error(std::string s) {
    std::cerr << message("Error: " + s) << std::endl;
}

/// Prints a fatal error message and exits.
void fatal(std::string s) {
    std::cerr << message("Fatal Error: " + s + " Exiting...") << std::endl;
    exit(-1);
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

    int segmentBegin = 0;
    for (const auto &m: matches(source, R"(=\s*SLB_\w*\s*<?)")) {
        // Keep the part of the string up to the beginning of the pattern.
        target += mysubstr(source, segmentBegin, m.position());

        // After pattern or closing bracket if there was one.
        segmentBegin = matchBracket(source, m.position() + m.length());

        std::string forRemoval = mysubstr(source, m.position(), segmentBegin);

        // Check that we are in the right context: the next non-whitespace
        // character is either a comma or a closing bracket.
        auto nonspace = find_if(source.cbegin() + segmentBegin, source.cend(),
                                std::not1(std::ref(isspace)));
        if (*nonspace != ',' && *nonspace != '>')
            fatal("Undefined symbol " + forRemoval +
                    " not in the default template argument context.");

        std::cout << message("Removing: ") << forRemoval << std::endl;
    }
    target += mysubstr(source, segmentBegin, source.size() - 1);

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
    out << target << std::endl;
    return 0;
}
