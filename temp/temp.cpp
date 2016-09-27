#include <iostream>
#include <memory>
#include <set>
#include <unordered_set>
#include <vector>
//#include <pair>
#include <algorithm>
#include <iomanip>
#include <sstream>

struct A {
    int x;
};

template <class T>
struct B {
    B(T &a) : a_(a) {}
    T &a_;
};

template <class T>
void f(B<T> &b) {
    b.a_.x = 5;
}

int main() {
    A a{5};
    B<const A> b{a};
    f(b);
    return 0;
}
