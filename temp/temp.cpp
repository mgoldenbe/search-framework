#include "type.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <map>

enum class NUMS {ONE, TWO};

class A {
public:
    NUMS n;
};


int main() {
    A a;
    //a.n = NUMS();
    std::cout << static_cast<int>(a.n) << std::endl;
    return 0;
}
