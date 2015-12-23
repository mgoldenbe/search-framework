#include <curses.h>
#include <menu.h>
#include <vector>
#include <string>
#include <map>

int main() {
    enum class A {E1, E2};
    std::map<A, std::pair<A, std::vector<std::string>>> myMap;
    myMap[A::E1] = {A::E2, {"abc", "bcd"}};
    return 0;
}
