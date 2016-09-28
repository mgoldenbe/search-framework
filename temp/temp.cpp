#include <iostream>
#include <memory>
#include <set>
#include <unordered_set>
#include <vector>
//#include <pair>
#include <algorithm>
#include <iomanip>
#include <sstream>

template <class T>
struct A {
    bool x = T::flag;
};

template <bool flag_>
struct B {
    static const bool flag = flag_;
};

template <bool flag_> struct C : B <flag_> {};

int main() {
    A<C<false>> a; (void)a;
    return 0;
}
