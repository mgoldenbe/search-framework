#include <iostream>
#include <memory>
#include <set>
#include <unordered_set>
#include <vector>
//#include <pair>
#include <algorithm>
#include <iomanip>
#include <sstream>

#include <iostream>
#include <string>
#include <regex>
using namespace std;

std::vector<std::smatch> matches(std::string s, std::string exp) {
    std::vector<std::smatch> res;
    std::regex reg{exp};
    for (auto i = std::sregex_iterator(s.begin(), s.end(), std::regex{exp});
         i != std::sregex_iterator(); ++i) 
      res.push_back(*i);
    return res;
}

int main() {
    std::regex r(R"(\w)");
    std::string s("a,b,c,d,e,f,g");
    for(std::sregex_iterator i = std::sregex_iterator(s.begin(), s.end(), r);
                             i != std::sregex_iterator();
                             ++i)
    {
        std::smatch m = *i;
        std::cout << "Match value: " << m.str() << " at Position " << m.position() << '\n';
    }
    return 0;
}

