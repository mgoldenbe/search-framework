#include "type.hpp"
#include <iostream>
#include <vector>
#include <string>

struct A {
    constexpr A(): x(0), y(0), z(0) {}
    int x;
    int y;
    int z;
};

int main() {
    constexpr A a = {1,2,3};
    return 0;
}
