#include "type.hpp"
#include <iostream>
#include <vector>
#include <string>

#include <sstream>

enum class E { ONE, TWO };

int main() {
    E e = E();
    std::cout << (e == E::ONE) << std::endl;
    (void)e;
    return 0;
}
