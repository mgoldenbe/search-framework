#include <iostream>
#include <memory>
#include <set>
#include <unordered_set>
#include <vector>
//#include <pair>
#include <algorithm>
#include <iomanip>
#include <sstream>

struct A {};

struct B: A {};

struct MySet {
    MySet(std::initializer_list<A> list) : v_{list} {}

private:
    std::vector<A> v_;
};

int main() {
    A a;
    B b;
    MySet s{a, b};
    (void)s;
    return 0;
}
