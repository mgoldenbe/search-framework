#include <iostream>
#include <fstream>
#include <regex>

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

int main(int argc, char **argv) {
    if (argc < 2)
        throw std::invalid_argument("Need to supply the input file");
    std::string target;
    std::ifstream in(argv[1]);
    if (!in)
        throw std::invalid_argument("Could not open file " +
                                    std::string(argv[1]));
    std::string str((std::istreambuf_iterator<char>(in)),
                    std::istreambuf_iterator<char>());
    std::regex exp(R"(SLB_\w*(\s*<)?)");
    std::smatch res;

    // http://stackoverflow.com/a/35026140/2725810
    // A simpler way: http://stackoverflow.com/a/37259671/2725810
    std::string::const_iterator curStart( str.cbegin() );
    while (regex_search(curStart, str.cend(), res, exp)) {
      auto replaceBegin = curStart + res.position();
        target += std::string(curStart, replaceBegin) + "Nothing";
        curStart = matchBracket(str, replaceBegin + res.length());
	std::cout << std::string(replaceBegin, curStart) << " --> Nothing" << std::endl;
    }
    target += std::string(curStart, str.cend());
    std::ofstream out(argv[1]);
    out << target << std::endl;
    return 0;
}
