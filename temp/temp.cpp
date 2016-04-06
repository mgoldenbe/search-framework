#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cassert>
#include <functional>
#include <algorithm>
#include <chrono>
#include "PrettyPrint.h"

struct A {
    int x = 5;
};

int main() {
    std::vector<A> v(10);
    for (auto a : v) std::cout << a.x << std::endl;
    return 0;
}
