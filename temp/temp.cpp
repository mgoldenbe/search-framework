#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cassert>
#include <functional>
#include <algorithm>
#include <chrono>
#include "PrettyPrint.h"

template <typename T1, typename T2> struct A {
    template <typename AnotherT1>
    using MyTemplate = A<AnotherT1, T2>;
};

template<class SomeA> struct B {
    using MyType = typename SomeA::template MyTemplate<double>;
};

int main() {
    B<A<int, int>> b; (void)b;
}
