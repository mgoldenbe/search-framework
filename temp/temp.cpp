#include <iostream>
#include <memory>
#include <set>
#include <unordered_set>
#include <vector>
//#include <pair>
#include <algorithm>
#include <iomanip>
#include <sstream>

struct A{};

void f(const A &a) {
    (void)a;
    std::cout << "f &" << std::endl;
}

void f(A &&a) {
    (void)a;
    std::cout << "f &&" << std::endl;
}

void g(A &&a) {
    f(std::forward<A>(a));
}

int main() {
    g(A{});
    return 0;
}
