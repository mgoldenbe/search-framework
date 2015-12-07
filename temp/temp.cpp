#include "type.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <type_traits>
#include <sstream>

struct A {
    int x = 5;
};

std::ostream& operator<<(std::ostream& o, const A &a) {o << a.x; return o;}


int main() {
    std::ostringstream so;
    A a;
    so << a;
    std::cout << so.str() << std::endl;
    return 0;
}
