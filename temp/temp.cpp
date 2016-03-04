#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cassert>
#include <functional>
#include <algorithm>
#include "PrettyPrint.h"

template <typename U>
struct A {
    template <typename T>
    T f() {return T(0);}
};

template <typename V>
struct B : A<int> {
    using MyT = int;
    using Base = A<int>;
    using Base::f;
    MyT f() {return f<MyT>();}
};

int main()
{
    B<int> b;
    std::cout << b.f() << std::endl;
    return 0;
}
