#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <unordered_set>
//#include <pair>
#include <algorithm>
#include <sstream>
#include <iomanip>

struct A {
    A() {}
    A(const A &) {}
};

struct B : A {
    using A::A;
};

int main() {
    A a;
    B b;
    B c(a);
    return 0;
}
